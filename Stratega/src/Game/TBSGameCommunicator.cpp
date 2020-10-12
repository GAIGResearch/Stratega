#include <Game/TBSGameCommunicator.h>

namespace SGA
{
	void TBSGameCommunicator::init()
	{
		thread = std::thread(&Agent::runTBS, std::ref(*agent), std::ref(*this), game->getForwardModel());
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
		return game->running() && game->getState().currentPlayer == getPlayerID();
	}

	void TBSGameCommunicator::executeAction(Action<Vector2i> action) const
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
		// ToDo FogOfWar breaks winConditions
		//state.applyFogOfWar(getPlayerID());
		return state;
	}

	bool TBSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}