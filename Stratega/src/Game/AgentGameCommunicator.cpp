#include <Stratega/Game/AgentGameCommunicator.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	AgentGameCommunicator::AgentGameCommunicator(int playerID, RTSGame& newGame, std::unique_ptr<Agent> newAgent, std::mt19937 newRngEngine) :
		GameCommunicator{ playerID }, tbsGame(nullptr), rtsGame(&newGame), agent(std::move(newAgent)), rngEngine(newRngEngine)
	{
	}

	AgentGameCommunicator::AgentGameCommunicator(int playerID, TBSGame& newGame, std::unique_ptr<Agent> newAgent, std::mt19937 newRngEngine) :
		GameCommunicator{ playerID }, tbsGame(&newGame), rtsGame(nullptr), agent(std::move(newAgent)), rngEngine(newRngEngine)
	{
	}

	void AgentGameCommunicator::init()
	{		
		//Initialize TBS Game
		if(tbsGame)
		{
			// Copy the forwardModel but ensure that it contains a different actionSpace instance
			TBSForwardModel copy(tbsGame->getForwardModel());
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

	bool AgentGameCommunicator::isMyTurn() const
	{
		if(tbsGame)
		{
			return tbsGame->running() && tbsGame->getState().currentPlayer == getPlayerID() && !tbsGame->isGameOver();
		}
		else
		{
			return rtsGame->running() /*&& game->getState().currentPlayer == getPlayerID() */ && !rtsGame->isGameOver();
		}
		
	}

	void AgentGameCommunicator::executeAction(Action action) const
	{
		if(tbsGame)
		{
			tbsGame->addActionToExecute(action);
			// The agent's thread only continues once the action has been executed
			// This ensures that an agent is deterministic, otherwise it can happen that an agent works with an old gameState
			// TODO Should this be done here?
			while (tbsGame->isUpdatingState());
		}
		else
		{
			rtsGame->executeAction(action);
		}
	}

	GameState AgentGameCommunicator::getGameState() const
	{
		if(tbsGame)
		{
			auto state = tbsGame->getStateCopy();
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
		if(tbsGame)
		{
			return tbsGame->isGameOver();
		}
		else
		{
			return rtsGame->isGameOver();
		}
	}
}