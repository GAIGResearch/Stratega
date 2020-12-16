#include <Game/AbstractRTSGame.h>
namespace SGA
{
	AbstractRTSGame::AbstractRTSGame(std::unique_ptr<RTSGameState2> gameState, RTSAbstractForwardModel forwardModel, std::mt19937 rngEngine)
		: Game(rngEngine), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}

	const RTSGameState2& AbstractRTSGame::getState() const
	{
		return *gameState;
	}

	RTSGameState2 AbstractRTSGame::getStateCopy()
	{
		std::lock_guard<std::mutex> copyGuard(stateMutex);
		return *gameState;
	}

	void AbstractRTSGame::update(double deltaTime)
	{
		accumulatedTimeUpdate += deltaTime;
		accumulatedTimePrint += deltaTime;

		if (accumulatedTimeUpdate > forwardModel.deltaTime)
		{
			//Execute
			stateMutex.lock();
			forwardModel.advanceGameState(*gameState, forwardModel.getActionSpace().generateEndAction());

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

	void AbstractRTSGame::close()
	{
		Game::close();

		std::cout << "GAME IS FINISHED" << std::endl;
		//std::cout << "Winner ID: " << gameState->getWinnerID() << std::endl;
	}

	void AbstractRTSGame::executeAction(Action action)
	{
		if (action.isEndAction)
			return;

		std::lock_guard<std::mutex> stateGuard(stateMutex);
		forwardModel.advanceGameState(*gameState, action);
	}

	
}
