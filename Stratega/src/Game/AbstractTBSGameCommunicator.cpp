#include <Game/AbstractTBSGameCommunicator.h>
#include <Agent/Agent.h>

namespace SGA
{
	void AbstractTBSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		TBSAbstractForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runAbstractTBS, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void AbstractTBSGameCommunicator::setGame(AbstractTBSGame& newGame)
	{
		this->game = &newGame;
	}

	void AbstractTBSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	bool AbstractTBSGameCommunicator::isMyTurn() const
	{
		return game->running() && game->getState().currentPlayer == getPlayerID() && !game->isGameOver();
	}

	void AbstractTBSGameCommunicator::executeAction(Action action) const
	{
		game->addActionToExecute(action);
			// The agent's thread only continues once the action has been executed
			// This ensures that an agent is deterministic, otherwise it can happen that an agent works with an old gameState
			// TODO Should this be done here?
			while (game->isUpdatingState());
		}

		TBSGameState2 AbstractTBSGameCommunicator::getGameState() const
		{
			auto state = game->getStateCopy();
			//state.applyFogOfWar(getPlayerID());
			return state;
		}

		bool AbstractTBSGameCommunicator::isGameOver() const
		{
			return game->isGameOver();
		}
}