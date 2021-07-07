#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>

namespace SGA {
	class UseSpecialAbilityScript : public BaseActionScript
	{

	public:
		UseSpecialAbilityScript() : BaseActionScript() {};

		Action getAction(const GameState& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "UseSpecialAbilityScript"; };

	};
}
