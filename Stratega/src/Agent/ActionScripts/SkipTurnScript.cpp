#include <Stratega/Agent/ActionScripts/SkipTurnScript.h>

namespace SGA
{
	Action SkipTurnScript::getAction(GameState& gameState, std::vector<SGA::Action>& actionSpace) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}

	Action SkipTurnScript::getActionForUnit(GameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
	{
		return Action::createEndAction(gameState.currentPlayer);
	}
}
