#include <Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	TBSAction SkipTurnScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
	{
		const TBSAction endTurnAction{ TBSActionType::EndTurn, gameState.currentPlayer, -1, {}, -1 };		
		return endTurnAction;
	}

	TBSAction SkipTurnScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
		const TBSAction endTurnAction{ TBSActionType::EndTurn, gameState.currentPlayer, -1, {}, -1 };
		return endTurnAction;
	}
}
