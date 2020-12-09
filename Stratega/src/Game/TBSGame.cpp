#include <Game/TBSGame.h>
#include <thread>

namespace SGA
{
	TBSGame::TBSGame(std::unique_ptr<TBSGameState> gameState, SGA::TBSForwardModel forwardModel, std::mt19937 rngEngine)
		: Game(rngEngine), gameState(std::move(gameState)), forwardModel(std::move(forwardModel))
	{
	}

	const TBSGameState& TBSGame::getState() const
	{
		return *gameState;
	}

	TBSGameState TBSGame::getStateCopy()
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
		std::cout << "Winner ID: " << gameState->getWinnerID() << std::endl;
	}
	
	void TBSGame::executeAction(TBSAction action)
	{
		//Lock mutex while we are advancing the gameState
		mutexGameState.lock();
		forwardModel.advanceGameState(*gameState, action);
		mutexGameState.unlock();
		hasActionToExecute = false;
		updatingState = false;

		for(auto& com : communicators)
		{
			com->onGameStateAdvanced();
		}
	}

	void TBSGame::addActionToExecute(TBSAction action)
	{
		if (gameState->currentPlayer == action.playerID)
		{
			actionToExecute = action;
			hasActionToExecute = true;
			updatingState = true;
		}
	}
}
