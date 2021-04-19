#include <random>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>

namespace SGA
{
	void AbstractStateMCTSAgent::runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		//const auto processedForwardModel = parameters_.preprocessForwardModel(&forwardModel);
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				// Fetch state
				auto state = gameCommunicator.getGameState();

				if (parameters_.STATE_FACTORY == nullptr)
				{
					parameters_.STATE_FACTORY = std::make_unique<StateFactory>(state);
					auto testconfig = std::map<std::string, int>();
					for (const auto entry : parameters_.STATE_FACTORY->config.insertEntityParameters) 
					{	
						testconfig[entry.first] = 1;
					}
					
					parameters_.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(testconfig, state);
				}

				
				auto gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				
				
				parameters_.PLAYER_ID = gameState.currentPlayer;

				// if there is just one action and we don't spent the time on continuing our search
				// we just instantly return it
				// todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
				
				auto actionSpace = forwardModel.generateActions(state);
				if (actionSpace.size() == 1 || !parameters_.CONTINUE_PREVIOUS_SEARCH)
				{
					gameCommunicator.executeAction(actionSpace.at(0));
					rootNode = nullptr;
					previousActionIndex = -1;
				}
				else
				{
					/*
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
						rootNode = std::make_unique<AbstractMCTSNode>(*processedForwardModel, gameState);
					}
					*/
					auto abstractState = parameters_.STATE_FACTORY->createAbstractState(gameState);
					auto gameStateCopy(gameState);
					rootNode = std::make_unique<AbstractMCTSNode>(forwardModel, abstractState, gameState);

					/*
					// test expansion
					while (!rootNode->isFullyExpanded())
					{
						rootNode->expand(forwardModel, parameters_, gameCommunicator.getRNGEngine());
					}
					rootNode->print();
					auto bestAction = actionSpace[0];
					*/
					
					//params.printDetails();
					parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;
					rootNode->searchMCTS(forwardModel, parameters_, gameCommunicator.getRNGEngine());
					//rootNode->printTree();

					// get and store best action
					const int bestActionIndex = rootNode->mostVisitedAction(parameters_, gameCommunicator.getRNGEngine());
					auto bestAction = rootNode->actionSpace.at(bestActionIndex);

					previousActionIndex = (bestAction.actionTypeFlags == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
					

					// return best action
					gameCommunicator.executeAction(bestAction);

				}
			}
		}
	}

	void AbstractStateMCTSAgent::runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
	{
		auto lastExecution = std::chrono::high_resolution_clock::now();
		while (!gameCommunicator.isGameOver())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> deltaTime = now - lastExecution;
			if (deltaTime.count() >= 1)
			{
				auto state = gameCommunicator.getGameState();
				auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());
				std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
				int temp = actionDist(gameCommunicator.getRNGEngine());
				gameCommunicator.executeAction(actions.at(actionDist(gameCommunicator.getRNGEngine())));
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}
}