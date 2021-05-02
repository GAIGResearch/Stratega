#include <random>
#include <Stratega/Agent/RandomAgent.h>

namespace SGA
{
	ActionAssignment RandomAgent::computeAction(GameState state, EntityForwardModel& forwardModel, long /*timeBudgetMs*/)
	{
		// Generate all available actions
		auto actions = forwardModel.generateActions(state, getPlayerID());
		// Uniformly sample a action
		std::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
		auto actionIndex = actionDist(getRNGEngine());
		auto action = actions.at(actionIndex);
		// Return Action
		return ActionAssignment::fromSingleAction(action);
	}
}