#include <random>
#include <Stratega/Agent/RandomAgent.h>

namespace SGA
{
	void RandomAgent::init(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(timeBudgetMs + 5))
		{

		}
	}

	ActionAssignment RandomAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(timeBudgetMs+5))
		{

		}

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