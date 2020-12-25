#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class SkipTurnScript : public BaseActionScript
	{
	public:
		SkipTurnScript() : BaseActionScript() {};

		Action getAction(TBSGameState& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(TBSGameState& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "SkipTurnScript"; };

	};

}
