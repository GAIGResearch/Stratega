#include <Game/RTSGameCommunicator.h>
#include <Agent/Agent.h>

namespace SGA
{
	void RTSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		RTSForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runAbstractRTS, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void RTSGameCommunicator::setGame(RTSGame& newGame)
	{
		this->game = &newGame;
	}

	void RTSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	bool RTSGameCommunicator::isMyTurn() const
	{
		return game->running() /*&& game->getState().currentPlayer == getPlayerID() */ && !game->isGameOver();
	}

	void RTSGameCommunicator::executeAction(Action action) const
	{
		game->executeAction(action);
		
	}

	RTSGameState RTSGameCommunicator::getGameState() const
	{
		auto state = game->getStateCopy();
		//state.applyFogOfWar(getPlayerID());
		return state;
	}

	bool RTSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}
