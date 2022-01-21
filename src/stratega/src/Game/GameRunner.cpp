#include <cassert>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Game/TBSGameRunner.h>
#include <Stratega/Game/RTSGameRunner.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/GUI/GameRenderer.h>

namespace SGA
{
	GameRunner::GameRunner(const GameConfig& newConfig)
		: currentState(),
		  config(&newConfig),
		shouldCheckComputationTime(newConfig.shouldCheckComputationTime),
		budgetTimeMs(newConfig.budgetTimeMs),
		disqualificationBudgetTimeMs(newConfig.disqualificationBudgetTimeMs),
		maxNumberWarnings(newConfig.maxNumberWarnings),
		shouldCheckInitTime(newConfig.shouldCheckInitTime),
		initBudgetTimetMs(newConfig.initBudgetTimetMs),
		initDisqualificationBudgetTimeMs(newConfig.initDisqualificationBudgetTimeMs)
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

	void GameRunner::render(SGA::Vector2i& resolution)
	{
		ensureRendererInitialized(resolution);
		renderer->render();
	}

	void GameRunner::checkInitializationTime(std::chrono::milliseconds initializationTime, int playerID)
	{
		if (initializationTime.count() > initBudgetTimetMs && initializationTime.count() < initDisqualificationBudgetTimeMs)
		{
			playerWarnings[static_cast<size_t>(playerID)]++;
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

	void GameRunner::ensureRendererInitialized(SGA::Vector2i& resolution)
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
