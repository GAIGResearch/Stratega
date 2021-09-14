#include <random>
#include <stratega/Agent/ScriptedAgent.h>
#include <stratega/ForwardModel/ForwardModel.h>

namespace SGA
{
	ActionAssignment ScriptedAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		// Return the action selected by the script
		auto actions = forwardModel.generateActions(state, getPlayerID());
		return ActionAssignment::fromSingleAction(actionScript_->getAction(state, actions, getPlayerID()));
	}
}