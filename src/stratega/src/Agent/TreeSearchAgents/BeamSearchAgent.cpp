#include <Stratega/Agent/TreeSearchAgents/BeamSearchAgent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>


namespace SGA
{
	ActionAssignment BeamSearchAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
		TreeNode rootNode = TreeNode(*processedForwardModel, state, getPlayerID());

		auto actionSpace = rootNode.getActionSpace(forwardModel, getPlayerID());
		if (actionSpace.size() == 1)
		{
			return ActionAssignment::fromSingleAction(actionSpace.front());
		}
		else
		{	
			auto bestAction = beamSearch(*processedForwardModel, rootNode);
			return ActionAssignment::fromSingleAction(bestAction);
		}
	}

	Action BeamSearchAgent::beamSearch(ForwardModel& forwardModel, TreeNode& root)
	{
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
				newBestSimulations.erase(newBestSimulations.begin() + static_cast<int>(parameters_.PLAYER_BEAM_WIDTH), newBestSimulations.end());
			bestSimulations = newBestSimulations;
		}

		sort(bestSimulations.begin(), bestSimulations.end(), sortByValue);
		TreeNode* bestChild = bestSimulations.front();
		while (bestChild->parentNode != nullptr)
		{
			TreeNode* parent = bestChild->parentNode;
			if (parent->parentNode == nullptr)
			{
				return parent->getActionSpace(forwardModel, getPlayerID()).at(static_cast<size_t>(bestChild->childIndex));
			}
			bestChild = parent;
		}

		return root.getActionSpace(forwardModel, getPlayerID()).at(0);
	}

	bool BeamSearchAgent::sortByValue(const TreeNode* i, const TreeNode* j) { return i->value > j->value; }

	std::vector<TreeNode*> BeamSearchAgent::simulate(ForwardModel& forwardModel, TreeNode& node)
	{
		std::vector<TreeNode*> bestSimulations = std::vector<TreeNode*>();
		
		// fully expand the whole node
		while (node.expand(forwardModel, parameters_) != nullptr) {};
		
		// rate each child according to scoring function
		for (auto& i : node.children)
		{
			auto* child = i.get();
			child->value = parameters_.heuristic->evaluateGameState(forwardModel, child->gameState, parameters_.PLAYER_ID);
			bestSimulations.push_back(child);
		}

		// determine the best children
		sort(bestSimulations.begin(), bestSimulations.end(), sortByValue);

		// delete all other children.
		// !!!Note: the index of the child is no longer associated with the index of an action in the node's action space!!!
		if (bestSimulations.size() > parameters_.PLAYER_BEAM_WIDTH)
			bestSimulations.erase(bestSimulations.begin() + static_cast<int>(parameters_.PLAYER_BEAM_WIDTH), bestSimulations.end());
			
		return bestSimulations;
	}


	void BeamSearchAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		parameters_.PLAYER_ID = getPlayerID();
		if (parameters_.heuristic == nullptr)
		{
			parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
		}
	}
}
