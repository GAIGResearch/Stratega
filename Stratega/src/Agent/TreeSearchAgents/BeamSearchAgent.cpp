#include <Agent/TreeSearchAgents/BeamSearchAgent.h>


namespace SGA
{
	void BeamSearchAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{				
				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				
				TreeNode rootNode = TreeNode(forwardModel, gameState);

				if (rootNode.actionSpace->count() == 1)
				{
					gameCommunicator.executeAction(rootNode.actionSpace->getAction(0));
				} else
				{
					Action<Vector2i> bestAction = beamSearch(forwardModel, rootNode);
					gameCommunicator.executeAction(bestAction);
				}
			}
		}
	}

	Action<Vector2i> BeamSearchAgent::beamSearch(TBSForwardModel& forwardModel, TreeNode& root)
	{
		int me = root.gameState.currentPlayer;

		std::vector<TreeNode*> bestSimulations = Simulate(forwardModel, root);

		for (int i = 1; i < playerDepth; i++)
		{
			std::vector<TreeNode*> newBestSimulations = std::vector<TreeNode*>();
			
			for (TreeNode* child : bestSimulations)
			{
				std::vector<TreeNode*> childSims = Simulate(forwardModel, *child);
				for (TreeNode* childSim : childSims)
				{
					newBestSimulations.push_back(childSim);
				}
			}
			
			sort(newBestSimulations.begin(), newBestSimulations.end(), sortByValue);
			if (newBestSimulations.size() > playerBeamWith)
				newBestSimulations.erase(newBestSimulations.begin() + playerBeamWith, newBestSimulations.end());
			bestSimulations = newBestSimulations;
		}

		sort(bestSimulations.begin(), bestSimulations.end(), sortByValue);
		TreeNode* bestChild = bestSimulations.front();
		while (bestChild->parentNode != nullptr)
		{
			TreeNode* parent = bestChild->parentNode;
			if (parent->parentNode == nullptr)
			{
				return parent->actionSpace->getAction(bestChild->childIndex);
			}
			bestChild = parent;
		}

		return root.actionSpace->getAction(0);
	}

	bool BeamSearchAgent::sortByValue(const TreeNode* i, const TreeNode* j) { return i->value > j->value; }

	std::vector<TreeNode*> BeamSearchAgent::Simulate(TBSForwardModel& forwardModel, TreeNode& node)
	{
		std::vector<TreeNode*> bestSimulations = std::vector<TreeNode*>();
		// fully expand the whole node
		while (node.expand(forwardModel) != nullptr) {};
		
		// rate each child according to scoring function
		for (size_t i = 0; i < node.children.size(); i++)
		{
			auto child = node.children.at(i).get();
			child->value = heuristic.evaluateGameState(forwardModel, child->gameState);
			bestSimulations.push_back(child);
		}

		// determine the best children
		sort(bestSimulations.begin(), bestSimulations.end(), sortByValue);

		// delete all other children. !!!Note: the index of the child is no longer associated with the index of an action in the node's action space!!!
		if (bestSimulations.size() > playerBeamWith)
			bestSimulations.erase(bestSimulations.begin() + playerBeamWith, bestSimulations.end());
		return bestSimulations;
	}
}
