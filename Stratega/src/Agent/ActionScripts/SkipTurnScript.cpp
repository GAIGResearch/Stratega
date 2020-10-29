#include <Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action<Vector2i> SkipTurnScript::getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const
	{
		const Action<Vector2i> endTurnAction{ ActionType::EndTurn, gameState.currentPlayer, -1, {}, -1 };		
		return endTurnAction;
	}

	Action<Vector2i> SkipTurnScript::getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const
	{
		const Action<Vector2i> endTurnAction{ ActionType::EndTurn, gameState.currentPlayer, -1, {}, -1 };
		return endTurnAction;
	}
}
