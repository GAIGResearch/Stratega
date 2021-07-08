#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>



namespace SGA {
	class AttackClosestOpponentScript : public BaseActionScript
	{
	public:
		AttackClosestOpponentScript() : BaseActionScript(){};

		Action getAction(const GameState& gameState, const ForwardModel& forwardModel, int playerID) const override;
		Action getActionForUnit(const GameState& gameState, const ForwardModel& forwardModel, int playerID, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackClosestOpponentScript";  };

	private:
		static double minimalDistanceToOpponents(const Vector2f position, std::map<int, Vector2f>& unitPositions, const std::set<int>& opponentUnits);

	};

}
