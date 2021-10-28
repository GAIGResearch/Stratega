#include <Stratega/Agent/TreeSearchAgents/BFSAgent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

namespace SGA
{
	ActionAssignment BFSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
	{
		parameters_.resetCounters(timer);
		const auto actionSpace = forwardModel.generateActions(state, getPlayerID());

		// if there is just one action and we don't spent the time on continuing our search
		// instead we return it instantly
		if (actionSpace.size() == 1)
		{
			// forget about your last action index, because the opponent will move in between
			previousActionIndex = -1;
			rootNode = nullptr;

			return ActionAssignment::fromSingleAction(actionSpace.front());
		}
		else // else we run a full search
		{
			// ToDo Move preprocessing to init
			const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);

			// init rootNode and node lists
			init(*processedForwardModel, state);

			// perform BFS Search
			search(*processedForwardModel, openNodes);

			// retrieve best action
			const int bestActionIndex = getBestActionIdx(*processedForwardModel);
			auto action = rootNode->getActionSpace(forwardModel, getPlayerID()).at(static_cast<size_t>(bestActionIndex));
			// remember latest action in case the search should be continued
			previousActionIndex = parameters_.continuePreviousSearch && (action.getActionFlag() == ActionFlag::EndTickAction) ? bestActionIndex : -1;

			return ActionAssignment::fromSingleAction(action);
		}
	}

	void BFSAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		parameters_.PLAYER_ID = getPlayerID();
		if (parameters_.heuristic == nullptr)
			parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
		if (parameters_.budgetType == Budget::UNDEFINED)
			parameters_.budgetType = Budget::TIME;
	}

	/// <summary>
	/// - Either creates a new rootNode or selects a subtree from the previous search.
	/// - Initializes openNodes and knownLeaves accordingly.
	/// - Sets the playerID.
	/// </summary>
	/// <param name="forwardModel">the same forward model as used during the search</param>
	/// <param name="gameState">the current game-state</param>
	void BFSAgent::init(ForwardModel& forwardModel, GameState& gameState)
	{
		if (parameters_.continuePreviousSearch && previousActionIndex != -1)
		{
			// in case of a deterministic game we know that the previously simulated action
			// should result in the same game-state as we predicted
			rootNode = std::move(rootNode->children.at(static_cast<size_t>(previousActionIndex)));
			rootNode->parentNode = nullptr;	// release parent
			fillOpenNodeListWithLeaves();
		}
		else
		{
			// in case of non-deterministic games we don't know the outcome of our action
			// additionally, in case the opponent did something since our last search,
			// we don't know the moves and need to restart our search
			rootNode = std::make_unique<TreeNode>(forwardModel, gameState, getPlayerID());
			openNodes.clear();
			openNodes.push_back(rootNode.get());
			knownLeaves.clear();
		}
	}

	/// <summary>
	/// Iterate through all the game tree in BFS manner.
	/// Store all the nodes that have not been completely expanded into openNodes.
	/// Store nodes that represent leaves and have been completely expanded into knownLeaves.
	/// </summary>
	/// <param name="forwardModel">the same forward model as used during the search</param>
	/// <param name="nodes">list of known open nodes</param>
	void BFSAgent::search(ForwardModel& forwardModel, std::list<TreeNode*>& nodes)
	{		
		while (!parameters_.isBudgetOver())
		{
			TreeNode* child = nullptr;
			while (child == nullptr && !nodes.empty())
			{
				TreeNode* currentNode = nodes.front();
				child = currentNode->expand(forwardModel, parameters_);
				if (child == nullptr)
				{
					nodes.pop_front();	// node cannot be further expanded
					if (nodes.empty())	// all nodes have been explored
						break;
				}
				else
				{
					// sort child node into its respective group
					if (child->gameState.isGameOver())
					{
						knownLeaves.push_back(currentNode);
					}
					else
					{
						nodes.push_back(child);
					}
				}
			}

			parameters_.currentIterations++;

			if (child == nullptr) // tree fully explored
				break;
		}
	}

	/// <summary>
	/// In case the tree is reused, we fill the openNodes and knownLeaves list according to the selected subtree.
	/// </summary>
	void BFSAgent::fillOpenNodeListWithLeaves()
	{
		openNodes.clear();
		knownLeaves.clear();
		
		std::list<TreeNode*> candidateNodes = std::list<TreeNode*>();
		candidateNodes.push_back(rootNode.get());

		while (!candidateNodes.empty())
		{
			//error: useless cast to type ‘class std::__cxx11::list<SGA::TreeNode*>’ [-Werror=useless-cast]
			//std::list<TreeNode*> tmpNodes = std::list<TreeNode*>(candidateNodes);
			std::list<TreeNode*> tmpNodes = candidateNodes;
			candidateNodes.clear();
			for (TreeNode* node : tmpNodes)
			{
				if (node->gameState.isGameOver())
					knownLeaves.push_back(node);
				else if (!node->isFullyExpanded())
					openNodes.push_back(node);
							
				for (auto& i : node->children)
					candidateNodes.push_back(i.get());
				
			}
		}
	}

	/// <summary>
	/// Get the index of the first child on the path to the best leaf node.
	/// </summary>
	/// <param name="forwardModel">the same model as it has been used for the search</param>
	/// <returns>index of the best child node</returns>
	int BFSAgent::getBestActionIdx(ForwardModel& forwardModel)
	{
		// iterate over all openNodes since they represent the tree's leafs
		std::shared_ptr<StateHeuristic> heuristic = parameters_.heuristic;
		double bestHeuristicValue = -std::numeric_limits<double>::max();
		TreeNode* bestChild = rootNode.get();

		// all nodes in openNodes and knownLeaves represent the end of a search path and could be the best node
		for (TreeNode* node : openNodes)
		{
			const double value = heuristic->evaluateGameState(forwardModel, node->gameState, parameters_.PLAYER_ID);
			if (value > bestHeuristicValue)
			{
				bestHeuristicValue = value;
				bestChild = node;
			}
		}

		// repeat the same search for all knownLeaves
		for (TreeNode* node : knownLeaves)
		{
			const double value = heuristic->evaluateGameState(forwardModel, node->gameState, parameters_.PLAYER_ID);
			if (value > bestHeuristicValue)
			{
				bestHeuristicValue = value;
				bestChild = node;
			}
		}

		// get the first action that leads on the path to the best known child
		int actionIdxToExecute = 0;
		while (bestChild->parentNode != nullptr)
		{
			actionIdxToExecute = bestChild->childIndex;
			bestChild = bestChild->parentNode;
		}
		
		return actionIdxToExecute;
	}
}
