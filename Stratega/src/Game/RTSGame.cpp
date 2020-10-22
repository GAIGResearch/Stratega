#include <Game/RTSGame.h>

namespace SGA
{
	RTSGame::RTSGame(std::unique_ptr<RTSGameState> gameState, RTSForwardModel forwardModel, std::mt19937 engine):
		Game(engine), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}
	
	void RTSGame::update(double deltaTime)
	{
		accumulatedTimeUpdate += deltaTime;
		accumulatedTimePrint += deltaTime;
		
		if (accumulatedTimeUpdate > forwardModel.deltaTime)
		{
			//Execute
			stateMutex.lock();
			forwardModel.advanceGameState(*gameState, actionCache);
			
			stateMutex.unlock();

			for (auto& com : communicators)
			{
				com->onGameStateAdvanced();
			}

			// Clean up cache
			actionCache.clear();

			accumulatedTimeUpdate = 0;
			executionCount++;
		}

		if (accumulatedTimePrint >= 1)
		{
			std::cout << "Advanced " << executionCount << " times the forwardModel in " << accumulatedTimePrint << "s." << std::endl;
			for (const auto& unit : gameState->units)
			{
				std::cout << "Unit " << unit.unitID << " at position (" << unit.position.x << ", " << unit.position.y << ")" << std::endl;
			}
			

			executionCount = 0;
			accumulatedTimePrint = 0;
		}
	}

	void RTSGame::addAction(const Action<Vector2f>& action)
	{
		std::lock_guard<std::mutex> copyGuard(stateMutex);
		actionCache.emplace_back(action);
	}

	const RTSGameState& RTSGame::getState()
	{
		return *gameState;
	}
	
	RTSGameState RTSGame::getStateCopy()
	{
		std::lock_guard<std::mutex> copyGuard(stateMutex);
		return *gameState;
	}

}