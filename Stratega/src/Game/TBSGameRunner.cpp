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
			auto& currentAgent = agents[currentState->currentPlayer];
			if(currentAgent != nullptr) // Run the agent if the player is not controlled by the GUI
			{
				agentThread.startComputing(*currentAgent, *currentState, *forwardModel);
				// Render
				auto startTime = std::chrono::high_resolution_clock::now();
				while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(40))
				{
					tbsRenderer->render();
				}
				// Collect action - ToDO verify that the agent didnt crash/hit time limit
				auto results = agentThread.join();
				nextAction = results.actions;
			}
			else // Wait for the GUI to return an action
			{
				while (!tbsRenderer->isActionAvailable() && !renderer->isGameEndRequested())
				{
					renderer->render();
				}
				nextAction = tbsRenderer->getPlayerActions();
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

	void TBSGameRunner::runInternal(std::vector<std::shared_ptr<Agent>>& agents, GameObserver& observer)
	{
		while (!currentState->isGameOver)
		{
			auto& currentAgent = agents[currentState->currentPlayer];
			auto results = runAgent(*currentAgent, *currentState, *forwardModel);
			// ToDO verify that the agent didnt crash/hit time limit
			forwardModel->advanceGameState(*currentState, results.actions);
			observer.onGameStateAdvanced(*currentState, *forwardModel);
		}
	}
}
