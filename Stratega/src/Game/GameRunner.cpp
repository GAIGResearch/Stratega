#include <cassert>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Game/TBSGameRunner.h>
#include <Stratega/Game/RTSGameRunner.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/GUI/GameRenderer.h>

namespace SGA
{
	GameRunner::GameRunner(const GameConfig& config)
		: currentState(),
		  config(&config),
		shouldCheckComputationTime(config.shouldCheckComputationTime),
		shouldCheckInitTime(config.shouldCheckInitTime),
		budgetTimeMs(config.budgetTimeMs),
		disqualificationBudgetTimeMs(config.disqualificationBudgetTimeMs),
		maxNumberWarnings(config.maxNumberWarnings),
		initBudgetTimetMs(config.initBudgetTimetMs),
		initDisqualificationBudgetTimeMs(config.initDisqualificationBudgetTimeMs)
	{
		reset();
	}

	void GameRunner::reset()
	{
		
		currentState = config->generateGameState();
		forwardModel = config->forwardModel->clone();
	}

	void GameRunner::reset(int levelID)
	{
		playerWarnings.clear();
		currentState = config->generateGameState(levelID);
		forwardModel = config->forwardModel->clone();
	}

	void GameRunner::step(const ActionAssignment& actions)
	{
		// ToDo
		// assert(actions.validate(currentState))
		forwardModel->advanceGameState(*currentState, actions);
		if (renderer != nullptr)
		{
			renderer->update(*currentState);
		}
	}

	void GameRunner::render()
	{
		ensureRendererInitialized();
		renderer->render();
	}

	void GameRunner::run(std::vector<std::unique_ptr<Agent>>& agents, GameObserver* observer)
	{
		assert(agents.size() == currentState->players.size());
		if(observer == nullptr)
		{
			observer = defaultObserver;
		}
		
		try
		{		
			// Check that no player is controlled by a human
			for (int i = 0; i < agents.size(); i++)
			{
				if (agents[i] == nullptr)
				{
					throw std::runtime_error("No player can be controlled by a human in an arena.");
				}
				else
				{
					agents[i]->setPlayerID(i);
				}
			}

			initializeAgents(agents);
			observer->onGameStarted(*currentState, *forwardModel);
			runInternal(agents, *observer);
			observer->onGameFinished(*currentState, *forwardModel);
		}
		catch (const std::exception& ex)
		{
			std::cout << "Gamme runner crashed error: " << ex.what() << std::endl;
		}
	}

	void GameRunner::play(std::vector<std::unique_ptr<Agent>>& agents)
	{
		assert(agents.size() == currentState->players.size());
		try
		{
			// Check if a player is controlled by an human
			int humanIndex = GameRenderer::NO_PLAYER_ID;
			for (int i = 0; i < agents.size(); i++)
			{
				if (agents[i] == nullptr)
				{
					if (humanIndex != GameRenderer::NO_PLAYER_ID)
					{
						throw std::runtime_error("Only one player can be controlled by a human. Index " + std::to_string(humanIndex) + " is already empty.");
					}
					humanIndex = i;
				}
				else
				{
					agents[i]->setPlayerID(i);
				}
			}

			initializeAgents(agents);
			ensureRendererInitialized();
			renderer->setPlayerPointOfView(humanIndex);
			playInternal(agents, humanIndex);
		}
		catch (const std::exception& ex)
		{
			std::cout << "Gamme runner crashed error: " << ex.what() << std::endl;
		}
	}

	void GameRunner::checkInitializationTime(std::chrono::milliseconds initializationTime, int playerID)
	{
		if (initializationTime.count() > initBudgetTimetMs && initializationTime.count() < initDisqualificationBudgetTimeMs)
		{
			playerWarnings[currentState->currentPlayer]++;
			throw warning("WARNING: player " + std::to_string(playerID) + " has exceeded the initialization time");
		}
		else if (initializationTime.count() >= initDisqualificationBudgetTimeMs)
		{
			//Disqualify player for exceeding the initialization time
			currentState->getPlayer(currentState->currentPlayer)->canPlay = false;
			throw warning("Player " + std::to_string(playerID) + " disqualified for exceeding the initialization time");
		}
	}

	void GameRunner::initializeAgents(std::vector<std::unique_ptr<Agent>>& agents)
	{
		try
		{			
			for (auto& agent : agents)
			{
				//Init warning vector
				playerWarnings.emplace_back(0);

				if (agent != nullptr)
				{
					auto stateCopy(*currentState);
					stateCopy.applyFogOfWar(agent->getPlayerID());

					auto begin = std::chrono::high_resolution_clock::now();
					agent->init(std::move(stateCopy), *forwardModel, initBudgetTimetMs);
					auto end = std::chrono::high_resolution_clock::now();
					auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>
						(end - begin);					
					
					try
					{
						//Check the initialization time of the agent
						if (shouldCheckInitTime)
							checkInitializationTime(initTime, agent->getPlayerID());
					}
					catch (const warning& ex)
					{
						std::cout << "Agent initialization warning: " << ex.what() << std::endl;
					}
					catch (const std::exception& ex)
					{
						std::cout << "Agent initialization crashed error: " << ex.what() << std::endl;
						return;
					}
									
				}
			}
		}
		catch (const warning& ex)
		{
			std::cout << "Agent initialization warning: " << ex.what() << std::endl;
		}
		catch (const std::exception& ex)
		{
			std::cout << "Agent initialization crashed error: " << ex.what() << std::endl;
			return;
		}
	}

	const GameState& GameRunner::getGameState() const
	{
		return *currentState;
	}

	void GameRunner::ensureRendererInitialized()
	{
		if (renderer == nullptr)
		{
			renderer = createRenderer(currentState->gameType);
			renderer->init(*currentState, *config);
		}
	}

	std::unique_ptr<GameRunner> createGameRunner(const GameConfig& config)
	{
		if (config.gameType == GameType::TBS)
		{
			return std::make_unique<TBSGameRunner>(config);
		}
		if (config.gameType == GameType::RTS)
		{
			return std::make_unique<RTSGameRunner>(config);
		}

		assert(false);
		return nullptr;
	}

}
