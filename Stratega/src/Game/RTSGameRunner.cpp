#include <Stratega/Game/RTSGameRunner.h>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{
	RTSGameRunner::RTSGameRunner(const GameConfig& config)
		: GameRunner(config)
	{
		currentState->currentPlayer = -1;
	}

	void RTSGameRunner::playInternal(std::vector<std::unique_ptr<Agent>>& agents, int /*humanIndex*/)
	{
		std::vector<AgentThread> threads(agents.size());
		while (!currentState->isGameOver && !renderer->isGameEndRequested())
		{
			// Run agents
			for(size_t i = 0; i < agents.size(); i++)
			{
				if(agents[i] != nullptr)
				{
					threads[i].startComputing(*agents[i], *currentState, *forwardModel, budgetTimeMs);
				}				
			}

			// Render
			auto startTime = std::chrono::high_resolution_clock::now();
			while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(40))
			{
				renderer->render();
			}

			// Collect actions
			ActionAssignment nextActions;
			for (size_t i = 0; i < agents.size(); i++)
			{
				try
				{
					if (agents[i] != nullptr)
					{
						auto results = threads[i].join();
						//Check if agent throw exception and rethrow it
						if (results.error)
						{
							std::rethrow_exception(results.error);
						}
						//Check computation time
						if (shouldCheckComputationTime)
							if (checkComputationTime(results.computationTime, agents[i]->getPlayerID()))
							{
								nextActions.merge(results.actions);
							}
					}
					else
					{
						nextActions.merge(renderer->getPlayerActions());
					}
				}	
				catch (const std::exception& ex)
				{
					std::cout << "Agent error: " << ex.what() << std::endl;
					return;
				}
			}
			
			// Step
			forwardModel->advanceGameState(*currentState, nextActions);
			renderer->update(*currentState);
		}
	}

	void RTSGameRunner::runInternal(std::vector<std::unique_ptr<Agent>>& agents, GameObserver& observer)
	{
		std::vector<AgentThread> threads(agents.size());
		while (!currentState->isGameOver)
		{
			ActionAssignment nextActions;
			try
			{
				// Run agents
				for (size_t i = 0; i < agents.size(); i++)
				{
					if (agents[i] != nullptr)
					{
						threads[i].startComputing(*agents[i], *currentState, *forwardModel, budgetTimeMs);
					}
				}

				// Collect actions				
				for (size_t i = 0; i < agents.size(); i++)
				{					
					auto results = threads[i].join();
					//Check if agent throw exception and rethrow it
					if (results.error)
					{
						std::rethrow_exception(results.error);
					}

					//Check computation time
					if (shouldCheckComputationTime)
						if (checkComputationTime(results.computationTime, agents[i]->getPlayerID()))
						{
							nextActions.merge(results.actions);
						}

				}
			}
			catch (const std::exception& ex)
			{
				std::cout << "Agent error: " << ex.what() << std::endl;
				return;
			}						

			// Step
			forwardModel->advanceGameState(*currentState, nextActions);
			observer.onGameStateAdvanced(*currentState, *forwardModel);
		}
	}

	bool  RTSGameRunner::checkComputationTime(std::chrono::milliseconds computationTime, int currentPlayerID)
	{
		if (playerWarnings[currentPlayerID] >= maxNumberWarnings)
		{
			//Disqualify player for exceeding the warning number
			currentState->getPlayer(currentPlayerID)->canPlay = false;
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " disqualified for exceeding warnings number" << std::endl;
			return false;
		}
		else if (computationTime.count() > budgetTimeMs && computationTime.count() < disqualificationBudgetTimeMs)
		{
			//add one warning
			playerWarnings[currentPlayerID]++;
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " has exceeded the computation time" << std::endl;
			return true;
		}
		else if (computationTime.count() >= disqualificationBudgetTimeMs)
		{
			//Disqualify player for exceeding the computation time
			currentState->getPlayer(currentPlayerID)->canPlay = false;
			std::cout << "WARNING: Player " << std::to_string(currentPlayerID) << " disqualified for exceeding the computation time" << std::endl;
			return false;
		}		
	}
}