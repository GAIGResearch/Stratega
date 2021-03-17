#include <Stratega/Game/TBSGameCommunicator.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	TBSGameCommunicator::TBSGameCommunicator(int playerID):
		GameCommunicator{playerID}, game(nullptr)
	{
	}

	void TBSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		TBSForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runTBS, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void TBSGameCommunicator::close()
	{
		thread.join();
	}

	std::mt19937& TBSGameCommunicator::getRNGEngine()
	{
		return rngEngine;
	}

	void TBSGameCommunicator::setGame(TBSGame& newGame)
	{
		this->game = &newGame;
	}

	void TBSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	void TBSGameCommunicator::setRNGEngine(std::mt19937 engine)
	{
		rngEngine = engine;
	}

	bool TBSGameCommunicator::isMyTurn() const
	{
		return game->running() && game->getState().currentPlayer == getPlayerID() && !game->isGameOver();
	}

	void TBSGameCommunicator::executeAction(Action action) const
	{
		game->addActionToExecute(action);
			// The agent's thread only continues once the action has been executed
			// This ensures that an agent is deterministic, otherwise it can happen that an agent works with an old gameState
			// TODO Should this be done here?
			while (game->isUpdatingState());
		}

	GameState TBSGameCommunicator::getGameState() const
	{
		auto state = game->getStateCopy();
		state.applyFogOfWar(getPlayerID());
		return state;
	}

	bool TBSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}