#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	ActionAssignment DoNothingAgent::computeAction(GameState /*state*/, const EntityForwardModel& /*forwardModel*/, long /*timeBudgetMs*/)
	{
		return ActionAssignment::fromSingleAction(Action::createEndAction(getPlayerID()));
	}
}