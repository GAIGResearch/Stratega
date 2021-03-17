#include <Stratega/Agent/TreeSearchAgents/MCTSAgent.h>

namespace SGA
{
	void MCTSAgent::runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(&forwardModel);
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
                auto gameState = gameCommunicator.getGameState();
                if (gameState.isGameOver)
                    break;
				
                const auto actionSpace = forwardModel.generateActions(gameState);
                parameters_.PLAYER_ID = gameState.currentPlayer;
				
                // if there is just one action and we don't spent the time on continuing our search
                // we just instantly return it
                // todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
                if (actionSpace.size() == 1 || !parameters_.CONTINUE_PREVIOUS_SEARCH)
                {
                    gameCommunicator.executeAction(actionSpace.at(0));
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
                    //rootNode->printTree();

                	// get and store best action
                    const int bestActionIndex = rootNode->mostVisitedAction(parameters_, gameCommunicator.getRNGEngine());
                    auto bestAction = rootNode->actionSpace.at(bestActionIndex);
                    gameCommunicator.executeAction(bestAction);

                	// return best action
                    previousActionIndex = (bestAction.actionTypeFlags == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
                }

			}
		}
	}
}