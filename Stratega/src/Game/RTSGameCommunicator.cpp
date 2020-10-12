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

	void RTSGameCommunicator::executeAction(Action<Vector2f> action) const
	{
		game->addAction(action);
	}

	RTSGameState RTSGameCommunicator::getGameState() const
	{
		return game->getStateCopy();
	}

	bool RTSGameCommunicator::isGameOver() const
	{
		return game->isGameOver();
	}
}