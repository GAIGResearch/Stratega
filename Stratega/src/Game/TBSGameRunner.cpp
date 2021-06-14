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

	void TBSGameRunner::playInternal(std::vector<std::unique_ptr<Agent>>& agents, int /*humanIndex*/)
	{
		auto* tbsRenderer = dynamic_cast<TBSGameRenderer*>(renderer.get());
		AgentThread agentThread;
		while(!currentState->isGameOver)
		{
			ActionAssignment nextAction;
			auto& currentAgent = agents[currentState->currentPlayer];
			if(currentAgent != nullptr) // Run the agent if the player is not controlled by the GUI
			{
				try
				{
					agentThread.startComputing(*currentAgent, *currentState, *forwardModel, timeBudgetMs);
					// Render
					auto startTime = std::chrono::high_resolution_clock::now();
					while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(timeBudgetMs))
					{
						tbsRenderer->render();
					}
					// Collect action - ToDO verify that the agent didnt crash/hit time limit
					auto results = agentThread.join();

					//Check if agent throw exception and rethrow it
					if (results.error)
					{
						std::rethrow_exception(results.error);
					}					

					nextAction = results.actions;
				}
				catch (const std::exception& ex)
				{
					std::cout << "Agent crashed error: " << ex.what() << std::endl;
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
	}

	void TBSGameRunner::runInternal(std::vector<std::unique_ptr<Agent>>& agents, GameObserver& observer)
	{
		while (!currentState->isGameOver)
		{
			AgentResults results;

			try
			{
				auto& currentAgent = agents[currentState->currentPlayer];
				results = runAgent(*currentAgent, *currentState, *forwardModel, timeBudgetMs);

				//Check if agent throw exception and rethrow it
				if (results.error)
				{
					std::rethrow_exception(results.error);
				}
			}			
			catch (const std::exception& ex)
			{
				std::cout << "Agent crashed error: " << ex.what() << std::endl;
				return;
			}

			// ToDO verify that the agent didnt hit time limit			
			forwardModel->advanceGameState(*currentState, results.actions);
			observer.onGameStateAdvanced(*currentState, *forwardModel);
		}
	}
}
