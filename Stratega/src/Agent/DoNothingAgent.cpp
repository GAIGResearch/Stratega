#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	ActionAssignment DoNothingAgent::computeAction(GameState /*state*/, EntityForwardModel& /*forwardModel*/, long /*timeBudgetMs*/)
	{
		return ActionAssignment::fromSingleAction(Action::createEndAction(getPlayerID()));
	}
}