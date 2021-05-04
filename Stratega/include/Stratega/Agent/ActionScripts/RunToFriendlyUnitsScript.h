#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>


namespace SGA {
	class RunToFriendlyUnitsScript : public BaseActionScript
	{

	public:
		RunToFriendlyUnitsScript() : BaseActionScript() {};

		Action getAction(const GameState& gameState, std::vector<Action>& actionSpace) const override;
		Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RunToFriendlyUnitsScript"; };


	private:
		static double maximalDistanceToFriends(const Vector2f position, std::map<int, Vector2f>& unitPositions, const std::set<int>& friendlyUnits);

	};

}
