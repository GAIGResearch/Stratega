#include <boost/random.hpp>
#include <Stratega/Agent/RandomAgent.h>

namespace SGA
{
	ActionAssignment RandomAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		// Generate all available actions
		auto actions = forwardModel.generateActions(state, getPlayerID());
		// Uniformly sample a action

		boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
		auto actionIndex = actionDist(getRNGEngine());
		auto action = actions.at(actionIndex);
		// std::cout << "	RandomActionIndex to execute: " << actionIndex<<" From: "<< actions.size() << std::endl;
		// Return Action
		return ActionAssignment::fromSingleAction(action);
	}
}