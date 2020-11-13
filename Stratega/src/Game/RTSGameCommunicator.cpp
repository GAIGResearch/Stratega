#include <Game/RTSGameCommunicator.h>

namespace SGA
{
	void RTSGameCommunicator::init()
	{
		thread = std::thread(&Agent::runRTS, std::ref(*agent), std::ref(*this), game->getForwardModel());
	}

	void RTSGameCommunicator::setGame(RTSGame& newGame)
	{
		this->game = &newGame;
	}

	void RTSGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	void RTSGameCommunicator::executeAction(RTSAction action) const
	{
		game->executeAction(action);
	}

	RTSGameState RTSGameCommunicator::getGameState() const
	{
		auto state = game->getStateCopy();
		state.applyFogOfWar(getPlayerID());
		return state;
	}

	bool RTSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}