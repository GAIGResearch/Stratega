#include <Stratega/Game/TBSGameRunner.h>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{
	TBSGameRunner::TBSGameRunner(const GameConfig& newConfig)
		: GameRunner(newConfig)
	{
	}

	void TBSGameRunner::playInternal(std::vector<Agent*>& agents, int /*humanIndex*/)
	{
		AgentThread agentThread;
		while(!currentState->isGameOver())
		{
			ActionAssignment nextAction;
			auto& currentAgent = agents[static_cast<size_t>(currentState->getCurrentTBSPlayer())];
			if(currentAgent != nullptr) // Run the agent if the player is not controlled by the GUI
			{
				try
				{
					agentThread.startComputing(*currentAgent, *currentState, *forwardModel, *config, budgetTimeMs);
					// Render
					while (agentThread.isComputing())
					{
						renderer->render();
					}

					auto results = agentThread.join();

					//Check if agent throw exception and rethrow it
					if (results.error)
						std::rethrow_exception(results.error);

					nextAction = results.actions;

					//Check computation time
					if (shouldCheckComputationTime)					
						if (!checkComputationTime(results.computationTime))
							nextAction = ActionAssignment::fromSingleAction(Action::createEndAction(currentAgent->getPlayerID()));
						
				}				
				catch (const std::exception& ex)
				{
					std::cout << "Agent error: " << ex.what() << std::endl;
					return;
				}				
			}
			else // Wait for the GUI to return an action
			{
				try
				{
					while (renderer->isWaiting() && !renderer->isGameEndRequested())
					{
						renderer->render();
					}
					nextAction = renderer->getPlayerActions();
				}
				catch (const std::exception& ex)
				{
					std::cout << "GUI crashed error: " << ex.what() << std::endl;
					return;
				}
			}

			// Stop game immediately
			if (renderer->isGameEndRequested())
			{
				break;
			}

			// Step
			forwardModel->advanceGameState(*currentState, nextAction);
			renderer->update(*currentState);
		}
        std::cout<<"WinnerID: "<< currentState->getWinnerID() << "\n";

		renderer->closeWindow();
	}

	void TBSGameRunner::runInternal(std::vector<Agent*>& agents, GameObserver& observer)
	{
		while (!currentState->isGameOver())
		{
			AgentResults results;
			ActionAssignment actionAssignment;
			//auto& currentAgent = agents[static_cast<size_t>(currentState->getCurrentTBSPlayer())];
			//results = runAgent(*currentAgent, *currentState, *forwardModel, *config, budgetTimeMs);

			////Check if agent throw exception and rethrow it
			//if (results.error)
			//	std::rethrow_exception(results.error);

			//actionAssignment = results.actions;
			////Check computation time
			//if (shouldCheckComputationTime)
			//	if (!checkComputationTime(results.computationTime))
			//		actionAssignment = ActionAssignment::fromSingleAction(Action::createEndAction(currentAgent->getPlayerID()));

			try
			{
				auto& currentAgent = agents[static_cast<size_t>(currentState->getCurrentTBSPlayer())];
				results = runAgent(*currentAgent, *currentState, *forwardModel, *config ,budgetTimeMs);

				//Check if agent throw exception and rethrow it
				if (results.error)
					std::rethrow_exception(results.error);

				actionAssignment = results.actions;
				//Check computation time
				if (shouldCheckComputationTime)
					if (!checkComputationTime(results.computationTime))
						actionAssignment = ActionAssignment::fromSingleAction(Action::createEndAction(currentAgent->getPlayerID()));						
			}
			catch (const std::exception& ex)
			{
				std::cout << "Agent error: " << ex.what() << std::endl;
				return;
			}
		
			forwardModel->advanceGameState(*currentState, actionAssignment);
			observer.onGameStateAdvanced(*currentState, *forwardModel);
		}
	}

	bool TBSGameRunner::checkComputationTime(std::chrono::milliseconds computationTime)
	{
		int currentPlayerID = currentState->getCurrentTBSPlayer();
		int budgetTimeWarningLimit = static_cast<int>(static_cast<float>(budgetTimeMs) * 1.05f);
		int disqualificationBudgetTimeMsLimit = static_cast<int>(static_cast<float>(disqualificationBudgetTimeMs) * 1.05f);


		if (playerWarnings[static_cast<size_t>(currentState->getCurrentTBSPlayer())] >= maxNumberWarnings)
		{
			//Disqualify player for exceeding the warning number
			currentState->getPlayer(currentPlayerID)->setCanPlay(false);
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " disqualified for exceeding warnings (" << maxNumberWarnings << ")" << std::endl;
			return false;
		}
		else if (computationTime.count() > static_cast<long long>(budgetTimeWarningLimit) && computationTime.count() < static_cast<long long>(disqualificationBudgetTimeMs))
		{
			//add one warning 
			playerWarnings[static_cast<size_t>(currentPlayerID)]++;
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " has exceeded the computation time (" << computationTime.count()
				<< ">" << budgetTimeWarningLimit << ")" << std::endl;
			return true;
		}
		else if (computationTime.count() >= static_cast<long long>(disqualificationBudgetTimeMsLimit))
		{
			//Disqualify player for exceeding the computation time
			currentState->getPlayer(currentPlayerID)->setCanPlay(false);
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " disqualified for exceeding the computation time (" << computationTime.count()
				<< ">" << disqualificationBudgetTimeMsLimit << ")" << std::endl;
			return false;
		}

		return true;
	}
}
