#include <random>
#include <Stratega/Agent/ScriptedAgent.h>

namespace SGA
{
	ActionAssignment ScriptedAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs)
	{
		// Generate all available actions
		auto actionSpace = forwardModel.generateActions(state, getPlayerID());

		// Return the action selected by the script
		return ActionAssignment::fromSingleAction(actionScript_->getAction(state, actionSpace));
	}
}