#include <Stratega/Game/AgentGameCommunicator.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	AgentGameCommunicator::AgentGameCommunicator(int playerID):
		GameCommunicator{playerID}, game(nullptr), rtsGame(nullptr)
	{
	}

	void AgentGameCommunicator::init()
	{
		//Initialize TBS Game
		if(game)
		{
			// Copy the forwardModel but ensure that it contains a different actionSpace instance
			TBSForwardModel copy(game->getForwardModel());
			copy.setActionSpace(copy.generateDefaultActionSpace());

			thread = std::thread(&Agent::runTBS, std::ref(*agent), std::ref(*this), std::move(copy));
		}
		
		//Initialize RTS Game
		if(rtsGame)
		{
			// Copy the forwardModel but ensure that it contains a different actionSpace instance
			RTSForwardModel copy2(rtsGame->getForwardModel());
			copy2.setActionSpace(copy2.generateDefaultActionSpace());

			thread = std::thread(&Agent::runRTS, std::ref(*agent), std::ref(*this), std::move(copy2));
		}		
	}

	void AgentGameCommunicator::close()
	{
		thread.join();
	}

	std::mt19937& AgentGameCommunicator::getRNGEngine()
	{
		return rngEngine;
	}

	void AgentGameCommunicator::setGame(TBSGame& newGame)
	{
		this->game = &newGame;
	}

	void AgentGameCommunicator::setGame(RTSGame& newGame)
	{
		this->rtsGame = &newGame;
	}

	void AgentGameCommunicator::setAgent(std::unique_ptr<Agent> agent)
	{
		this->agent = std::move(agent);
	}

	void AgentGameCommunicator::setRNGEngine(std::mt19937 engine)
	{
		rngEngine = engine;
	}

	bool AgentGameCommunicator::isMyTurn() const
	{
		if(game)
		{
			return game->running() && game->getState().currentPlayer == getPlayerID() && !game->isGameOver();
		}
		else
		{
			return rtsGame->running() /*&& game->getState().currentPlayer == getPlayerID() */ && !rtsGame->isGameOver();
		}
		
	}

	void AgentGameCommunicator::executeAction(Action action) const
	{
		if(game)
		{
			game->addActionToExecute(action);
			// The agent's thread only continues once the action has been executed
			// This ensures that an agent is deterministic, otherwise it can happen that an agent works with an old gameState
			// TODO Should this be done here?
			while (game->isUpdatingState());
		}
		else
		{
			rtsGame->executeAction(action);
		}
	}

	GameState AgentGameCommunicator::getGameState() const
	{
		if(game)
		{
			auto state = game->getStateCopy();
			state.applyFogOfWar(getPlayerID());
			return state;
		}
		else
		{
			auto state = rtsGame->getStateCopy();
			state.applyFogOfWar(getPlayerID());
			return state;
		}
	}

	
	bool AgentGameCommunicator::isGameOver() const
	{
		if(game)
		{
			return game->isGameOver();
		}
		else
		{
			return rtsGame->isGameOver();
		}
	}
}