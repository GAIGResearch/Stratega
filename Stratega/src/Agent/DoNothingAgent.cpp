#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	ActionAssignment DoNothingAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long /*timeBudgetMs*/)
	{
		if(state.gameType==GameType::RTS)
			return ActionAssignment();
		else
			return ActionAssignment::createEndActionAssignment(getPlayerID());
	}
}