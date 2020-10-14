#include <Agent/TreeSearchAgents/MCTSAgent.h>

namespace SGA
{
	void MCTSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
                TBSGameState gameState = gameCommunicator.getGameState();
                if (gameState.isGameOver)
                    break;
				
                auto actionSpace = forwardModel.getActions(gameState);
                // Initialization
				/*
                if (rootNode == nullptr)
                {
                    rootNode = std::make_unique<MCTSNode>(forwardModel, gameState);
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
                else
                {
                    if (continuePreviousSearch && previousActionIndex != -1 && gameState.currentGameTurn == playerTurn)
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
                        rootNode = std::make_unique<MCTSNode>(forwardModel, gameState);
                        playerTurn = gameState.currentGameTurn;
                    }
                	
                    MCTSParams params = MCTSParams();
                    params.playerID = gameCommunicator.getPlayerID();
                    //params.printDetails();
                	
                    rootNode->searchMCTS(forwardModel, params, gameCommunicator.getRNGEngine());
                    //std::cout << "Remaining ForwardModel Calls" <<  params.REMAINING_FM_CALLS;

                    const int bestActionIndex = rootNode->mostVisitedAction(params, gameCommunicator.getRNGEngine());
                    gameCommunicator.executeAction(rootNode->actionSpace->getAction(bestActionIndex));
                    if (bestActionIndex < actionSpace->count() - 1)
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
}