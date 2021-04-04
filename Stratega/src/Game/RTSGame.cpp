#include <Stratega/Game/RTSGame.h>
namespace SGA
{
	RTSGame::RTSGame(std::unique_ptr<GameState> gameState, RTSForwardModel forwardModel, std::mt19937 rngEngine)
		: Game(rngEngine), actions(), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}

	const GameState& RTSGame::getState() const
	{
		return *gameState;
	}

	GameState RTSGame::getStateCopy()
	{
		std::lock_guard<std::mutex> copyGuard(stateMutex);
		return *gameState;
	}

	void RTSGame::update(double deltaTime)
	{
		accumulatedTimeUpdate += deltaTime;
		accumulatedTimePrint += deltaTime;

		if (accumulatedTimeUpdate > forwardModel.deltaTime)
		{
			//Execute
			stateMutex.lock();
			forwardModel.advanceGameState(*gameState, actions);
			actions.clear();

			//Update navmesh if it needs to
			if (shouldUpdateNavmesh)
			{
				forwardModel.buildNavMesh(*gameState, navigationConfig);
				shouldUpdateNavmesh = false;
			}

			stateMutex.unlock();

			for (auto& com : communicators)
			{
				com->onGameStateAdvanced();
			}

			accumulatedTimeUpdate = 0;
			executionCount++;
		}

		if (accumulatedTimePrint >= 1)
		{
			std::cout << "Advanced " << executionCount << " times the forwardModel in " << accumulatedTimePrint << "s." << std::endl;
			for (const auto& unit : gameState->entities)
			{
				std::cout << "Unit " << unit.id << " at position (" << unit.position.x << ", " << unit.position.y << ")" << std::endl;
			}


			executionCount = 0;
			accumulatedTimePrint = 0;
		}
	}

	void RTSGame::close()
	{
		Game::close();

		std::cout << "GAME IS FINISHED" << std::endl;
		//std::cout << "Winner ID: " << gameState->getWinnerID() << std::endl;
	}

	void RTSGame::executeAction(Action action)
	{
		if (action.actionTypeFlags == ActionFlag::EndTickAction)
			return;

		std::lock_guard<std::mutex> stateGuard(stateMutex);
		actions.emplace_back(action);
	}

	
}
