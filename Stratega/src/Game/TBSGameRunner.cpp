#pragma once
#include <cassert>
#include <Stratega/Game/TBSGameRunner.h>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{
	TBSGameRunner::TBSGameRunner(std::unique_ptr<GameConfig> config)
		: config(std::move(config))
	{
		reset();
	}


	void TBSGameRunner::reset()
	{
		currentState = config->generateGameState();
		// ToDo Copy
		forwardModel = std::move(config->forwardModel);
	}

	void TBSGameRunner::step(const ActionAssignment& actions)
	{
		// ToDo
		// assert(actions.validate(currentState))
		forwardModel->advanceGameState(*currentState, actions);
		if(renderer != nullptr)
		{
			renderer->update(*currentState);
		}
	}

	void TBSGameRunner::render()
	{
		ensureRendererInitialized();
		renderer->render();
		// ToDo
	}

	void TBSGameRunner::play(std::vector<std::unique_ptr<Agent>>& agents)
	{
		assert(agents.size() == currentState->players.size());

		// Check if a player is controlled by an human
		int humanIndex = GameRenderer::NO_PLAYER_ID;
		for(int i = 0; i < agents.size(); i++)
		{
			if (agents[i] == nullptr)
			{
				if(humanIndex != GameRenderer::NO_PLAYER_ID)
				{
					throw std::runtime_error("Only one player can be controlled by a human. Index " + std::to_string(humanIndex) + " is already empty.");
				}
				humanIndex = i;
			}
		}

		ensureRendererInitialized();
		renderer->setPlayerPointOfView(humanIndex);
		
		AgentThread agentThread;
		while(!currentState->isGameOver)
		{
			ActionAssignment nextAction;
			auto& currentAgent = agents[currentState->currentPlayer];
			if(currentAgent != nullptr) // Run the agent if the player is not controlled by the GUI
			{
				agentThread.startComputing(*currentAgent, *currentState);
				// Render
				auto startTime = std::chrono::high_resolution_clock::now();
				while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(40))
				{
					renderer->render();
				}
				// Collect action - ToDO verify that the agent didnt crash/hit time limit
				auto results = agentThread.join();
				nextAction = results.actions;
			}
			else // Wait for the GUI to return an action
			{
				while (!renderer->isActionAvailable())
				{
					renderer->render();
				}
				nextAction = renderer->getPlayerActions();
			}

			// Step
			forwardModel->advanceGameState(*currentState, nextAction);
			renderer->update(*currentState);
		}
	}

	void TBSGameRunner::run(std::vector<std::unique_ptr<Agent>>& agents)
	{
		assert(agents.size() == currentState->players.size());
	}

	void TBSGameRunner::ensureRendererInitialized()
	{
		if(renderer == nullptr)
		{
			renderer = std::make_unique<TBSGameRenderer>();
			renderer->init(*currentState, *config);
		}
	}

	const GameState& TBSGameRunner::getGameState() const
	{
		return *currentState;
	}

}
