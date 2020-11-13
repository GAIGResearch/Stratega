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
			forwardModel.advanceGameState(*gameState, forwardModel.generateEndTickAction());

			//Update navmesh if it needs to
			if(shouldUpdateNavmesh)
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
			for (const auto& unit : gameState->units)
			{
				std::cout << "Unit " << unit.unitID << " at position (" << unit.position.x << ", " << unit.position.y << ")" << std::endl;
			}
			

			executionCount = 0;
			accumulatedTimePrint = 0;
		}
	}

	void RTSGame::close()
	{
		Game::close();
		std::cout << "GAME IS FINISHED" << std::endl;
		std::cout << "Winner ID: " << gameState->winnerPlayerID << std::endl;
	}

	void RTSGame::executeAction(const RTSAction& action)
	{
		if (action.type == RTSActionType::EndTick)
			return;
		
		std::lock_guard<std::mutex> stateGuard(stateMutex);
		forwardModel.advanceGameState(*gameState, action);
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