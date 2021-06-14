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
		while (!currentState->isGameOver && !renderer->isGameEndRequested())
		{
			// Run agents
			for(size_t i = 0; i < agents.size(); i++)
			{
				if(agents[i] != nullptr)
				{
					threads[i].startComputing(*agents[i], *currentState, *forwardModel, timeBudgetMs);
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
						// ToDO verify that the agent didnt hit time limit
						nextAction.merge(results.actions);
					}
					else
					{
						nextAction.merge(renderer->getPlayerActions());
					}
				}				
				catch (const std::exception& ex)
				{
					std::cout << "Agent crashed error: " << ex.what() << std::endl;
					return;
				}
			}
			
			// Step
			forwardModel->advanceGameState(*currentState, nextAction);
			renderer->update(*currentState);
		}
	}

	void RTSGameRunner::runInternal(std::vector<std::unique_ptr<Agent>>& agents, GameObserver& observer)
	{
		std::vector<AgentThread> threads(agents.size());
		while (!currentState->isGameOver)
		{
			ActionAssignment nextActions;
			try {
				// Run agents
				for (size_t i = 0; i < agents.size(); i++)
				{
					if (agents[i] != nullptr)
					{
						threads[i].startComputing(*agents[i], *currentState, *forwardModel, timeBudgetMs);
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
					// ToDO verify that the agent didnt hit time limit
					nextActions.merge(results.actions);
				}
			}
			catch (const std::exception& ex)
			{
				std::cout << "Agent crashed error: " << ex.what() << std::endl;
				return;
			}

			// Step
			forwardModel->advanceGameState(*currentState, nextActions);
			observer.onGameStateAdvanced(*currentState, *forwardModel);
		}
	}
}