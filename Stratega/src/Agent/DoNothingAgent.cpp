#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	ActionAssignment DoNothingAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
	{
		if(state.gameType==GameType::RTS)
			return ActionAssignment();
		else
			return ActionAssignment::createEndActionAssignment(getPlayerID());
	}
}