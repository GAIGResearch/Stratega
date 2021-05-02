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
		  config(&config)
	{
		reset();
	}

	void GameRunner::reset()
	{
		currentState = config->generateGameState();
		// ToDo Copy
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

	void GameRunner::run(std::vector<std::unique_ptr<Agent>>& agents)
	{
		assert(agents.size() == currentState->players.size());
		
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

		runInternal(agents);
	}

	void GameRunner::play(std::vector<std::unique_ptr<Agent>>& agents)
	{
		assert(agents.size() == currentState->players.size());

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

		ensureRendererInitialized();
		renderer->setPlayerPointOfView(humanIndex);
		playInternal(agents, humanIndex);
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
