#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>


namespace SGA {
	class RandomActionScript : public BaseActionScript
	{

	public:
		RandomActionScript() : BaseActionScript() {};

		TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const override;
		TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RandomActionScript"; };
		[[nodiscard]] int getID() const override { return 6; };

	};
}
