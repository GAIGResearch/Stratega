#include <Agent/TreeSearchAgents/BFSAgent.h>

#include "ForwardModel/PortfolioTBSForwardModel.h"

namespace SGA
{
	void BFSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(&forwardModel);

		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)	//safety check against race conditions
					break;
				const auto actionSpace = forwardModel.getActions(gameState);
				
				// if there is just one action and we don't spent the time on continuing our search
				// instead we return it instantly
				if (actionSpace->count() == 1)
				{
					gameCommunicator.executeAction(actionSpace->getAction(0));
					
					// forget about your last action index, because the opponent will move in between
					previousActionIndex = -1;
					rootNode = nullptr;
				}
				else // else we run a full search
				{
					// init rootNode and node lists
					init(processedForwardModel.get(), gameState);

					// perform BFS Search
					search(processedForwardModel.get(), openNodes);

					// retrieve best action
					const int bestActionIndex = getBestActionIdx(processedForwardModel.get());
					auto action = rootNode->actionSpace->getAction(bestActionIndex);
					gameCommunicator.executeAction(action);
					
					// remember latest action in case the search should be continued
					previousActionIndex = parameters_.CONTINUE_PREVIOUS_SEARCH && action.getType() != ActionType::EndTurn ? bestActionIndex : -1;

				}
			}
		}
	}

	/// <summary>
	/// - Either creates a new rootNode or selects a subtree from the previous search.
	/// - Initializes openNodes and knownLeaves accordingly.
	/// - Sets the playerID.
	/// </summary>
	/// <param name="forwardModel">the same forward model as used during the search</param>
	/// <param name="gameState">the current game-state</param>
	void BFSAgent::init(TBSForwardModel* forwardModel, TBSGameState& gameState)
	{
		parameters_.PLAYER_ID = gameState.currentPlayer;
		if (parameters_.CONTINUE_PREVIOUS_SEARCH && previousActionIndex != -1)
		{
			// in case of a deterministic game we know that the previously simulated action
			// should result in the same game-state as we predicted
			rootNode = std::move(rootNode->children.at(previousActionIndex));
			rootNode->parentNode = nullptr;	// release parent
			fillOpenNodeListWithLeaves();
		}
		else
		{
			// in case of non-deterministic games we don't know the outcome of our action
			// additionally, in case the opponent did something since our last search,
			// we don't know the moves and need to restart our search
			rootNode = std::make_unique<TreeNode>(forwardModel, gameState);
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
	/// <param name="openNodes">list of known open nodes</param>
	void BFSAgent::search(TBSForwardModel* forwardModel, std::list<TreeNode*>& openNodes)
	{
		parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;
		
		while (parameters_.REMAINING_FM_CALLS > 0)
		{
			TreeNode* child = nullptr;
			while (child == nullptr && !openNodes.empty())
			{
				TreeNode* currentNode = openNodes.front();
				child = currentNode->expand(forwardModel, parameters_);
				if (child == nullptr)
				{
					openNodes.pop_front();	// node cannot be further expanded
					if (openNodes.empty())	// all nodes have been explored
						break;
				}
				else
				{
					// sort child node into its respective group
					if (child->gameState.isGameOver)
					{
						knownLeaves.push_back(currentNode);
					}
					else
					{
						openNodes.push_back(child);
					}
				}
			}

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
			std::list<TreeNode*> tmpNodes = std::list<TreeNode*>(candidateNodes);
			candidateNodes.clear();
			for (TreeNode* node : tmpNodes)
			{
				
				if (node->gameState.isGameOver)
				{
					knownLeaves.push_back(node);
				}
				else
				{
					if (node->children.size() != node->actionSpace->count())
					{
						openNodes.push_back(node);
					}
				}

				for (auto& i : node->children)
				{
					candidateNodes.push_back(i.get());
				}
			}
		}
	}

	/// <summary>
	/// Get the index of the first child on the path to the best leaf node.
	/// </summary>
	/// <param name="forwardModel">the same model as it has been used for the search</param>
	/// <returns>index of the best child node</returns>
	int BFSAgent::getBestActionIdx(TBSForwardModel* forwardModel)
	{
		// iterate over all openNodes since they represent the tree's leafs
		StateHeuristic* heuristic = parameters_.OBJECTIVE.get();
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
