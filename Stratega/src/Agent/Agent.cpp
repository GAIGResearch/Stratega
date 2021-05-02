#include <Stratega/Agent/Agent.h>

namespace SGA
{
	Agent::Agent()
		: playerID(-1), rngEngine(std::random_device{}())
	{
	}

	void Agent::init(GameState /*initialState*/, const EntityForwardModel& /*forwardModel*/, long /*timeBudgetMs*/)
	{
		// Do nothing by default
	}

	int Agent::getPlayerID()
	{
		return playerID;
	}

	void Agent::setPlayerID(int playerID)
	{
		this->playerID = playerID;
	}
	
	void Agent::setSeed(std::mt19937::result_type seed)
	{
		rngEngine.seed(seed);
	}

	std::mt19937& Agent::getRNGEngine()
	{
		return rngEngine;
	}


}