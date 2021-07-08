#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class SkipTurnScript : public BaseActionScript
	{
	public:
		SkipTurnScript() : BaseActionScript() {};

		Action getAction(const GameState& gameState, const ForwardModel& forwardModel, int playerID) const override;
		Action getActionForUnit(const GameState& gameState, const ForwardModel& forwardModel, int playerID, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "SkipTurnScript"; };

	};

}
