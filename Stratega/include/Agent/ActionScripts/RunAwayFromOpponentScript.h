#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>
#include <Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>


namespace SGA {
	class RunAwayFromOpponentScript : public BaseActionScript
	{

	public:
		RunAwayFromOpponentScript() : BaseActionScript() {};

		Action<Vector2i> getAction(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RunAwayFromOpponentScript"; };

	private:
		static int maximalDistanceToOpponents(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits);

	};

}
