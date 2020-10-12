#include <Agent/TreeSearchAgents/BFSAgent.h>

namespace SGA
{
	void BFSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{				
				TBSGameState gameState = gameCommunicator.getGameState();
				const auto actionSpace = forwardModel.getActions(gameState);

				/*
				if (rootNode == nullptr)
				{
					rootNode = std::make_unique<TreeNode>(forwardModel, gameState);
				}*/

				// if there is just one action and we don't spent the time on continuouing our search
				// we just instantly return it
				// todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
				if (actionSpace->count() == 1 || !continuePreviousSearch)
				{
					gameCommunicator.executeAction(actionSpace->getAction(0));
					rootNode = nullptr;
					previousActionIndex = -1;
				}
				else // else we run a full search
				{
					if (continuePreviousSearch && previousActionIndex != -1 && gameState.currentGameTurn == playerTurn)
					{
						// in case of deterministic games we know which move has been done by us
						rootNode = std::move(rootNode->children.at(previousActionIndex));
						rootNode->parentNode = nullptr;	// release parent
						fillOpenNodeListWithLeaves();
					}
					else
					{
						// in case of non-deterministic games we don't know the outcome of our action
						// we don't know the moves of our opponent
						// todo compare current gameState with states in the tree
						rootNode = std::make_unique<TreeNode>(forwardModel, gameState);
						openNodes.clear();
						openNodes.push_back(rootNode.get());
						playerTurn = gameState.currentGameTurn;

					}
					search(forwardModel, openNodes);
										
					// iterate over all openNodes since they represent the tree's leafs
					LinearSumHeuristic heuristic = LinearSumHeuristic();
					double bestHeuristicValue = -std::numeric_limits<double>::max();
					TreeNode* bestChild = rootNode.get();

					for (TreeNode* node : openNodes)
					{
						double value = heuristic.evaluateGameState(forwardModel, node->gameState);
						if (value > bestHeuristicValue)
						{
							bestHeuristicValue = value;
							bestChild = node;
						}
					}

					// return the first action on the path to the best leaf
					int bestActionIndex = 0;
					while (bestChild->parentNode != nullptr)
					{
						bestActionIndex = bestChild->childIndex;
						bestChild = bestChild->parentNode;
					}

					gameCommunicator.executeAction(rootNode->actionSpace->getAction(bestActionIndex));
					if (bestActionIndex < actionSpace->count()-1)
					{
						previousActionIndex = bestActionIndex;
					}
					else
					{
						previousActionIndex = -1;
					}					
				}
			}
		}
	}

	void BFSAgent::search(TBSForwardModel& forwardModel, std::list<TreeNode*>& openNodes) const
	{
		for (int i = 0; i < forwardModelCalls; i++)
		{
			TreeNode* child = nullptr;
			while (child == nullptr)
			{
				TreeNode* currentNode = openNodes.front();
				child = currentNode->expand(forwardModel);
				if (child == nullptr)
				{
					openNodes.pop_front();	//node cannot be further expanded
					if (openNodes.empty())
						break;
					currentNode = openNodes.front();
				}
				else
				{
					openNodes.push_back(child);
				}
			}

			if (child == nullptr) // tree fully explored
				break;
		}
	}

	void BFSAgent::fillOpenNodeListWithLeaves()
	{
		openNodes.clear();
		std::list<TreeNode*> candidateNodes = std::list<TreeNode*>();
		candidateNodes.push_back(rootNode.get());

		while (!candidateNodes.empty())
		{
			std::list<TreeNode*> tmpNodes = std::list<TreeNode*>(candidateNodes);
			candidateNodes.clear();
			for (TreeNode* node : tmpNodes)
			{
				if (node->children.size() != node->actionSpace->count())
				{
					openNodes.push_back(node);
				}

				for (size_t i = 0; i < node->children.size(); i++)
				{
					candidateNodes.push_back(node->children.at(i).get());
				}
			}
		}
	}

	
	
}
