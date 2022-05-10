#include <Stratega/Agent/Agent.h>

namespace SGA
{
	Agent::Agent(const std::string& name)
		: playerID(-1), rngEngine(), agentName(name)
	{
	}

	void Agent::init(GameState /*initialState*/, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		// Do nothing by default
	}

	int Agent::getPlayerID()
	{
		return playerID;
	}

	void Agent::setPlayerID(int newPlayerID)
	{
		this->playerID = newPlayerID;
	}
	
	void Agent::setSeed(boost::mt19937::result_type seed)
	{
		rngEngine.seed(seed);
	}

	boost::mt19937& Agent::getRNGEngine()
	{
		return rngEngine;
	}

	std::vector<Action> Agent::filterActionsByTypeID(const std::vector<Action>& actions, int typeID) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
			if (ac.getActionTypeID() == typeID)
				fActions.emplace_back(ac);
		return fActions;
	}

	std::vector<Action> Agent::filterActionsByName(const std::vector<Action>& actions, std::string name) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
			if (ac.getActionName() == name)
				fActions.emplace_back(ac);
		return fActions;
	}

	std::vector<Action> Agent::filterActionsByFlag(const std::vector<Action>& actions, ActionFlag flag) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
			if (ac.getActionFlag() == flag)
				fActions.emplace_back(ac);
		return fActions;
	}

	std::vector<Action> Agent::filterActionsBySource(const std::vector<Action>& actions, ActionSourceType source) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
			if (ac.getActionSourceType() == source)
				fActions.emplace_back(ac);
		return fActions;
	}


	std::vector<Action> Agent::filterActionsByEntityID(const std::vector<Action>& actions, int entityID) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
		{
			if (ac.getTargets().size() > 0 && (ac.getTargets()[0].getType() == ActionTarget::Type::EntityReference))
			{
				if (ac.getTargets()[0].getEntityID() == entityID)
					fActions.emplace_back(ac);
			}
		}		
		return fActions;
	}

	std::vector<Action> Agent::filterActionsByPlayerID(const std::vector<Action>& actions, int searchedPlayerID) const
	{
		std::vector<Action> fActions;
		for (Action ac : actions)
		{
			if (ac.getTargets().size() > 0 && (ac.getTargets()[0].getType() == ActionTarget::Type::PlayerReference))
			{
				if (ac.getTargets()[0].getPlayerID() == searchedPlayerID)
					fActions.emplace_back(ac);
			}
		}
		return fActions;
	}
}