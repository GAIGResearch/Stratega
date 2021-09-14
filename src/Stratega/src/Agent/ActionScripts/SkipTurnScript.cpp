#include <Stratega/Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action SkipTurnScript::getAction(const GameState& /*gameState*/, std::vector<Action>& /*actionSpace*/, int playerID) const
	{
		return Action::createEndAction(playerID);
	}

	Action SkipTurnScript::getActionForUnit(const GameState& /*gameState*/, std::vector<Action>& /*actionSpace*/, int playerID, int) const
	{
		return Action::createEndAction(playerID);
	}
}
