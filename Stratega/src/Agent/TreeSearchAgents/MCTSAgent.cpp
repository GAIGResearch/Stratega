#include <Agent/TreeSearchAgents/MCTSAgent.h>

namespace SGA
{
	void MCTSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(&forwardModel);
        int i = 0;
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
                TBSGameState gameState = gameCommunicator.getGameState();
                if (gameState.isGameOver)
                    break;
                i++;
				
                const auto actionSpace = forwardModel.getActions(gameState);
                parameters_.PLAYER_ID = gameState.currentPlayer;
				
                // if there is just one action and we don't spent the time on continueing our search
                // we just instantly return it
                // todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
                if (actionSpace->count() == 1 || !parameters_.CONTINUE_PREVIOUS_SEARCH)
                {
                    gameCommunicator.executeAction(actionSpace->getAction(0));
                    rootNode = nullptr;
                    previousActionIndex = -1;
                }
                else
                {
                    if (parameters_.CONTINUE_PREVIOUS_SEARCH && previousActionIndex != -1)
                    {
                        // in case of deterministic games we know which move has been done by us
                    	// reuse the tree from the previous iteration
                        rootNode = std::move(rootNode->children.at(previousActionIndex));
                        rootNode->parentNode = nullptr;	// release parent
                        rootNode->setDepth(0);
                    }
                    else
                    {
						// start a new tree
                        rootNode = std::make_unique<MCTSNode>(*processedForwardModel, gameState);
                    }
                	
                    //params.printDetails();
                    parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;
                    rootNode->searchMCTS(*processedForwardModel, parameters_, gameCommunicator.getRNGEngine());
                    //std::cout << "Remaining ForwardModel Calls " <<  parameters_.REMAINING_FM_CALLS << std::endl;
                    rootNode->printTree();

                    const int bestActionIndex = rootNode->mostVisitedAction(parameters_, gameCommunicator.getRNGEngine());
                    auto bestAction = rootNode->actionSpace->getAction(bestActionIndex);

                    if (i == 3)
                    {
                    	//rootNode has 8 units
                        std::cout << "rootNode gameState: " << std::endl;
                        rootNode->gameState.printGameStateStatus();

                    	//predicted gamestate has 8 units
                        std::cout << "child gameState: " << std::endl;
                        rootNode->children[bestActionIndex]->gameState.printGameStateStatus();

                    	//follow-up game-state has 7 units, because one of the player's units walked on a hole and died
                    }
                	
                	gameCommunicator.executeAction(bestAction);
                    previousActionIndex = bestAction.getType() == ActionType::EndTurn ? (-1) : bestActionIndex;
                }

			}
		}
	}
}