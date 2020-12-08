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
		std::lock_guard<std::mutex> copyGuard(mutexGameState);
		return *gameState;
	}

	void AbstractRTSGame::update(double deltaTime)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (hasActionToExecute)
		{
			executeAction(actionToExecute);
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

	void AbstractRTSGame::addActionToExecute(Action action)
	{
		/*if (gameState->currentPlayer == action.owner)
		{
			actionToExecute = action;
			hasActionToExecute = true;
			updatingState = true;
		}*/
	}
}