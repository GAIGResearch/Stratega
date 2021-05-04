#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>



namespace SGA {
	class AttackClosestOpponentScript : public BaseActionScript
	{

	public:
		AttackClosestOpponentScript() : BaseActionScript(){};

		Action getAction(const GameState& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackClosestOpponentScript";  };

	private:
		static int minimalDistanceToOpponents(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits);

	};

}
