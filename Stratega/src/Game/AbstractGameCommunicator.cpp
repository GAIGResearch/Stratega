#include <Game/AbstractGameCommunicator.h>

namespace SGA
{
	void AbstractGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		TBSAbstractForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runAbstract, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void AbstractGameCommunicator::setGame(AbstractGame& newGame)
	{
		this->game = &newGame;
	}

	void AbstractGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	bool AbstractGameCommunicator::isMyTurn() const
	{
		return game->running() && game->getState().currentPlayer == getPlayerID() && !game->isGameOver();
	}

	void AbstractGameCommunicator::executeAction(Action action) const
	{
		game->addActionToExecute(action);
			// The agent's thread only continues once the action has been executed
			// This ensures that an agent is deterministic, otherwise it can happen that an agent works with an old gameState
			// TODO Should this be done here?
			while (game->isUpdatingState());
		}

		TBSGameState2 AbstractGameCommunicator::getGameState() const
		{
			auto state = game->getStateCopy();
			//state.applyFogOfWar(getPlayerID());
			return state;
		}

		bool AbstractGameCommunicator::isGameOver() const
		{
			return game->isGameOver();
		}
}