#include <Stratega/Game/RTSGameRunner.h>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{
	RTSGameRunner::RTSGameRunner(const GameConfig& config)
		: GameRunner(config)
	{
	}

	void RTSGameRunner::playInternal(std::vector<std::unique_ptr<Agent>>& agents, int /*humanIndex*/)
	{
		std::vector<AgentThread> threads(agents.size());
		while (!currentState->isGameOver)
		{
			// Run agents
			for(size_t i = 0; i < agents.size(); i++)
			{
				if(agents[i] != nullptr)
				{
					threads[i].startComputing(*agents[i], *currentState, *forwardModel);
				}				
			}

			// Render
			auto startTime = std::chrono::high_resolution_clock::now();
			while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(40))
			{
				renderer->render();
			}

			// Collect actions
			ActionAssignment nextAction;
			for (size_t i = 0; i < agents.size(); i++)
			{
				if (agents[i] != nullptr)
				{
					auto results = threads[i].join();
					// ToDO verify that the agent didnt crash/hit time limit
					nextAction.merge(results.actions);
				}
				else
				{
					nextAction.merge(renderer->getPlayerActions());
				}
			}
			
			// Step
			forwardModel->advanceGameState(*currentState, nextAction);
			renderer->update(*currentState);
		}
	}

	void RTSGameRunner::runInternal(std::vector<std::unique_ptr<Agent>>& agents)
	{
		std::vector<AgentThread> threads(agents.size());
		while (!currentState->isGameOver)
		{
			// Run agents
			for (size_t i = 0; i < agents.size(); i++)
			{
				if (agents[i] != nullptr)
				{
					threads[i].startComputing(*agents[i], *currentState, *forwardModel);
				}
			}

			// Collect actions
			ActionAssignment nextAction;
			for (size_t i = 0; i < agents.size(); i++)
			{
				auto results = threads[i].join();
				// ToDO verify that the agent didnt crash/hit time limit
				nextAction.merge(results.actions);
			}

			// Step
			forwardModel->advanceGameState(*currentState, nextAction);
			renderer->update(*currentState);
		}
	}



}