#include <Game/TBSGameCommunicator.h>
#include <Agent/Agent.h>

namespace SGA
{
	void TBSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		TBSForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());

		thread = std::thread(&Agent::runAbstractTBS, std::ref(*agent), std::ref(*this), std::move(copy));
	}

	void TBSGameCommunicator::setGame(TBSGame& newGame)
	{
		this->game = &newGame;
	}

	void TBSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
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

		TBSGameState TBSGameCommunicator::getGameState() const
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