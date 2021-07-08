#include <random>
#include <Stratega/Agent/ScriptedAgent.h>

namespace SGA
{
	ActionAssignment ScriptedAgent::computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs)
	{
		// Return the action selected by the script
		return ActionAssignment::fromSingleAction(actionScript_->getAction(state, *forwardModel, getPlayerID()));
	}
}