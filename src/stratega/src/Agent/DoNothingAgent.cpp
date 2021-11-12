#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	ActionAssignment DoNothingAgent::computeAction(GameState /*state*/, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		return ActionAssignment::createEndActionAssignment(getPlayerID());
	}
}