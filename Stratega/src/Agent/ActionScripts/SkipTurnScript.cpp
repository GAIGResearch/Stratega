#include <Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action SkipTurnScript::getAction(TBSGameState2& gameState, std::vector<SGA::Action>& actionSpace) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}

	Action SkipTurnScript::getActionForUnit(TBSGameState2& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}
}
