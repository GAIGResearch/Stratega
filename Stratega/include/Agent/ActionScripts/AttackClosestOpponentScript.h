#pragma once
#include <ForwardModel/Action.h>
#include <Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>


namespace SGA {
	class AttackClosestOpponentScript : public BaseActionScript
	{

	public:
		AttackClosestOpponentScript() : BaseActionScript(){};

		Action<Vector2i> getAction(TBSGameState& gameState, std::vector<Action<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::vector<Action<Vector2i>>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackClosestOpponentScript";  };

	private:
		static int minimalDistanceToOpponents(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits);

	};

}
