#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class SkipTurnScript : public BaseActionScript
	{
	public:
		SkipTurnScript() : BaseActionScript() {};

		Action getAction(TBSGameState2& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(TBSGameState2& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "SkipTurnScript"; };

	};

}
