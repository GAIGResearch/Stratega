#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>

namespace SGA {
	class UseSpecialAbilityScript : public BaseActionScript
	{

	public:
		UseSpecialAbilityScript() : BaseActionScript() {};

		TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const override;
		TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "UseSpecialAbilityScript"; };
	};
}
