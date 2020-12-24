#include <Game/AbstractRTSGameCommunicator.h>
namespace SGA
{
	void AbstractRTSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		RTSAbstractForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runAbstractRTS, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void AbstractRTSGameCommunicator::setGame(AbstractRTSGame& newGame)
	{
		this->game = &newGame;
	}

	void AbstractRTSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	bool AbstractRTSGameCommunicator::isMyTurn() const
	{
		return game->running() /*&& game->getState().currentPlayer == getPlayerID() */ && !game->isGameOver();
	}

	void AbstractRTSGameCommunicator::executeAction(Action action) const
	{
		game->executeAction(action);
		
	}

	RTSGameState2 AbstractRTSGameCommunicator::getGameState() const
	{
		auto state = game->getStateCopy();
		//state.applyFogOfWar(getPlayerID());
		return state;
	}

	bool AbstractRTSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}