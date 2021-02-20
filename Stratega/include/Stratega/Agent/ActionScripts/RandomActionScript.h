#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>


namespace SGA {
	class RandomActionScript : public BaseActionScript
	{

	public:
		RandomActionScript() : BaseActionScript() {};

		Action getAction(TBSGameState& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(TBSGameState& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RandomActionScript"; };
	};
}
