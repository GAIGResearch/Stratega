#include <Stratega/Game/TBSGame.h>
#include <thread>

namespace SGA
{
	TBSGame::TBSGame(std::unique_ptr<GameState> gameState, ForwardModel forwardModel, std::mt19937 rngEngine)
		: Game(rngEngine), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}

	const GameState& TBSGame::getState() const
	{
		return *gameState;
	}

	GameState TBSGame::getStateCopy()
	{
		std::lock_guard<std::mutex> copyGuard(mutexGameState);
		return *gameState;
	}

	void TBSGame::update(double deltaTime)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (hasActionToExecute)
		{
			executeAction(actionToExecute);
		}
	}

	void TBSGame::close()
	{
		Game::close();

		std::cout << "GAME IS FINISHED" << std::endl;
		//std::cout << "Winner ID: " << gameState->getWinnerID() << std::endl;
	}

	void TBSGame::executeAction(Action action)
	{
		//Lock mutex while we are advancing the gameState
		mutexGameState.lock();
		forwardModel.advanceGameState(*gameState, action);
		mutexGameState.unlock();
		hasActionToExecute = false;
		updatingState = false;

		for (auto& com : communicators)
		{
			com->onGameStateAdvanced();
		}
	}

	void TBSGame::addActionToExecute(Action action)
	{
		if (gameState->currentPlayer == action.ownerID)
		{
			actionToExecute = action;
			hasActionToExecute = true;
			updatingState = true;
		}
	}
}
