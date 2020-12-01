#include <Game/AbstractGame.h>

namespace SGA
{
	AbstractGame::AbstractGame(std::unique_ptr<TBSGameState2> gameState, SGA::ForwardModel<TBSGameState2> forwardModel, std::mt19937 rngEngine)
		: Game(rngEngine), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}

	const TBSGameState2& AbstractGame::getState() const
	{
		return *gameState;
	}

	TBSGameState2 AbstractGame::getStateCopy()
	{
		std::lock_guard<std::mutex> copyGuard(mutexGameState);
		return *gameState;
	}

	void AbstractGame::update(double deltaTime)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (hasActionToExecute)
		{
			executeAction(actionToExecute);
		}
	}

	void AbstractGame::close()
	{
		Game::close();

		std::cout << "GAME IS FINISHED" << std::endl;
		//std::cout << "Winner ID: " << gameState->getWinnerID() << std::endl;
	}

	void AbstractGame::executeAction(Action action)
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

	void AbstractGame::addActionToExecute(Action action)
	{
		if (gameState->currentPlayer == action.owner)
		{
			actionToExecute = action;
			hasActionToExecute = true;
			updatingState = true;
		}
	}
}
