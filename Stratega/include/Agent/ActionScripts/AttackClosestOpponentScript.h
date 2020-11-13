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

		TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const override;
		TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackClosestOpponentScript";  };

	private:
		static int minimalDistanceToOpponents(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits);

	};

}
