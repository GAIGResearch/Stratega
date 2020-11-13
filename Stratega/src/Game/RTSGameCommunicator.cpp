#include <Game/RTSGameCommunicator.h>

namespace SGA
{
	void RTSGameCommunicator::init()
	{
		// Copy the forwardModel but ensure that it contains a different actionSpace instance
		RTSForwardModel copy(game->getForwardModel());
		copy.setActionSpace(copy.generateDefaultActionSpace());
		
		thread = std::thread(&Agent::runRTS, std::ref(*agent), std::ref(*this), std::move(copy));
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