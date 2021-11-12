#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class SkipTurnScript : public BaseActionScript
	{
	public:
		SkipTurnScript() : BaseActionScript() {};

		Action getAction(const GameState& gameState, std::vector<Action>& actionSpace, int playerID) const override;
		Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int playerID, int unitID) const override;
		std::string toString() const override { return "SkipTurnScript"; };

	};

}
