#include <cassert>
#include <stratega/Game/GameRunner.h>
#include <stratega/Game/TBSGameRunner.h>
#include <stratega/Game/RTSGameRunner.h>
#include <stratega/Representation/GameState.h>
#include <stratega/ForwardModel/ForwardModel.h>
#include <stratega/Configuration/GameConfig.h>
#include <stratega/GUI/GameRenderer.h>

namespace SGA
{
	GameRunner::GameRunner(const GameConfig& config)
		: currentState(),
		  config(&config),
		shouldCheckComputationTime(config.shouldCheckComputationTime),
		budgetTimeMs(config.budgetTimeMs),
		disqualificationBudgetTimeMs(config.disqualificationBudgetTimeMs),
		maxNumberWarnings(config.maxNumberWarnings),
		shouldCheckInitTime(config.shouldCheckInitTime),
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

	void GameRunner::render(SGA::Vector2f& resolution)
	{
		ensureRendererInitialized(resolution);
		renderer->render();
	}

	void GameRunner::checkInitializationTime(std::chrono::milliseconds initializationTime, int playerID)
	{
		if (initializationTime.count() > initBudgetTimetMs && initializationTime.count() < initDisqualificationBudgetTimeMs)
		{
			playerWarnings[playerID]++;
			std::cout << "WARNING: Player " << std::to_string(playerID) << " has exceeded the initialization time" << std::endl;
		}
		else if (initializationTime.count() >= initDisqualificationBudgetTimeMs)
		{
			//Disqualify player for exceeding the initialization time
			currentState->getPlayer(playerID)->setCanPlay(false);
			std::cout << "WARNING: Player " << std::to_string(playerID) << " disqualified for exceeding the initialization time" << std::endl;
		}
	}

	void GameRunner::initializeAgents(std::vector<Agent*>& agents)
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

					if(config->applyFogOfWar)
						stateCopy.applyFogOfWar(agent->getPlayerID());

					auto begin = std::chrono::high_resolution_clock::now();
					agent->init(std::move(stateCopy), *forwardModel, Timer(initBudgetTimetMs));
					auto end = std::chrono::high_resolution_clock::now();
					auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>
						(end - begin);					
										
					//Check the initialization time of the agent
					if (shouldCheckInitTime)
						checkInitializationTime(initTime, agent->getPlayerID());

				}
			}
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

	void GameRunner::ensureRendererInitialized(SGA::Vector2f& resolution)
	{
		if (renderer == nullptr)
		{
			renderer = createRenderer(currentState->getGameType(), resolution);
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
