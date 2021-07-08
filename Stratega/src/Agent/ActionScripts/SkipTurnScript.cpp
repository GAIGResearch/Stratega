#include <Stratega/Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action SkipTurnScript::getAction(const GameState& gameState, const ForwardModel& forwardModel, int playerID) const
	{
		return Action::createEndAction(playerID);
	}

	Action SkipTurnScript::getActionForUnit(const GameState& gameState, const ForwardModel& forwardModel, int playerID, int) const
	{
		return Action::createEndAction(playerID);
	}
}
