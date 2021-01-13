#include <Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action SkipTurnScript::getAction(TBSGameState& gameState, std::vector<SGA::Action>& actionSpace) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}

	Action SkipTurnScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}
}
