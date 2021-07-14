#pragma once
#include <Stratega/Game/TBSGameRunner.h>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/GUI/TBSGameRenderer.h>

namespace SGA
{
	TBSGameRunner::TBSGameRunner(const GameConfig& config)
		: GameRunner(config)
	{
	}

	void TBSGameRunner::playInternal(std::vector<std::shared_ptr<Agent>>& agents, int /*humanIndex*/)
	{
		auto* tbsRenderer = dynamic_cast<TBSGameRenderer*>(renderer.get());
		AgentThread agentThread;
		while(!currentState->isGameOver)
		{
			ActionAssignment nextAction;
			auto& currentAgent = agents[currentState->getCurrentTBSPlayer()];
			if(currentAgent != nullptr) // Run the agent if the player is not controlled by the GUI
			{
				try
				{
					agentThread.startComputing(*currentAgent, *currentState, *forwardModel, *config, budgetTimeMs);
					// Render
					auto startTime = std::chrono::high_resolution_clock::now();
					while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(budgetTimeMs))
					{
						tbsRenderer->render();
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
					while (!tbsRenderer->isActionAvailable() && !renderer->isGameEndRequested())
					{
						renderer->render();
					}
					nextAction = tbsRenderer->getPlayerActions();
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

		tbsRenderer->closeWindow();
	}

	void TBSGameRunner::runInternal(std::vector<std::shared_ptr<Agent>>& agents, GameObserver& observer)
	{
		while (!currentState->isGameOver)
		{
			AgentResults results;
			ActionAssignment actionAssignment;
			try
			{
				auto& currentAgent = agents[currentState->getCurrentTBSPlayer()];
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
		if (playerWarnings[currentState->getCurrentTBSPlayer()] >= maxNumberWarnings)
		{
			//Disqualify player for exceeding the warning number
			currentState->getPlayer(currentState->getCurrentTBSPlayer())->canPlay = false;
			std::cout<<"WARNING: Player  " << std::to_string(currentState->getCurrentTBSPlayer()) << " disqualified for exceeding warnings number" << std::endl;
			return false;
		}
		else if (computationTime.count() > budgetTimeMs && computationTime.count() < disqualificationBudgetTimeMs)
		{
			//add one warning
			playerWarnings[currentState->getCurrentTBSPlayer()]++;
			std::cout<<"WARNING: Player " << std::to_string(currentState->getCurrentTBSPlayer()) << " has exceeded the computation time"<<std::endl;
			return true;
		}
		else if (computationTime.count() >= disqualificationBudgetTimeMs)
		{
			//Disqualify player for exceeding the computation time
			currentState->getPlayer(currentState->getCurrentTBSPlayer())->canPlay = false;
			std::cout<<"WARNING: Player " << std::to_string(currentState->getCurrentTBSPlayer()) << " disqualified for exceeding the computation time" << std::endl;
			return false;
		}		
	}
}
