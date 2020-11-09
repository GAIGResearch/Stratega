#include <Agent/TreeSearchAgents/BeamSearchAgent.h>


namespace SGA
{
	void BeamSearchAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(&forwardModel);

		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{				
				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				
				TreeNode rootNode = TreeNode(*processedForwardModel, gameState);

				if (rootNode.actionSpace->count() == 1)
				{
					gameCommunicator.executeAction(rootNode.actionSpace->getAction(0));
				} else
				{
					Action<Vector2i> bestAction = beamSearch(*processedForwardModel, rootNode);
					gameCommunicator.executeAction(bestAction);
				}
			}
		}
	}

	Action<Vector2i> BeamSearchAgent::beamSearch(TBSForwardModel& forwardModel, TreeNode& root)
	{
		parameters_.PLAYER_ID = root.gameState.currentPlayer;

		std::vector<TreeNode*> bestSimulations = simulate(forwardModel, root);

		for (size_t i = 1; i < parameters_.PLAYER_BEAM_DEPTH; i++)
		{
			std::vector<TreeNode*> newBestSimulations = std::vector<TreeNode*>();
			
			for (TreeNode* child : bestSimulations)
			{
				std::vector<TreeNode*> childSims = simulate(forwardModel, *child);
				for (TreeNode* childSim : childSims)
				{
					newBestSimulations.push_back(childSim);
				}
			}
			
			sort(newBestSimulations.begin(), newBestSimulations.end(), sortByValue);
			if (newBestSimulations.size() > parameters_.PLAYER_BEAM_WIDTH)
				newBestSimulations.erase(newBestSimulations.begin() + parameters_.PLAYER_BEAM_WIDTH, newBestSimulations.end());
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

	std::vector<TreeNode*> BeamSearchAgent::simulate(TBSForwardModel& forwardModel, TreeNode& node)
	{
		std::vector<TreeNode*> bestSimulations = std::vector<TreeNode*>();
		
		// fully expand the whole node
		while (node.expand(forwardModel, parameters_) != nullptr) {};
		
		// rate each child according to scoring function
		for (auto& i : node.children)
		{
			auto* child = i.get();
			child->value = parameters_.OBJECTIVE->evaluateGameState(forwardModel, child->gameState, parameters_.PLAYER_ID);
			bestSimulations.push_back(child);
		}

		// determine the best children
		sort(bestSimulations.begin(), bestSimulations.end(), sortByValue);

		// delete all other children.
		// !!!Note: the index of the child is no longer associated with the index of an action in the node's action space!!!
		if (bestSimulations.size() > parameters_.PLAYER_BEAM_WIDTH)
			bestSimulations.erase(bestSimulations.begin() + parameters_.PLAYER_BEAM_WIDTH, bestSimulations.end());
			
		return bestSimulations;
	}
}
