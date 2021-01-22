#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>


namespace SGA {
	class RunToFriendlyUnitsScript : public BaseActionScript
	{

	public:
		RunToFriendlyUnitsScript() : BaseActionScript() {};

		TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const override;
		TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RunToFriendlyUnitsScript"; };
		[[nodiscard]] int getID() const override { return 3; };


	private:
		static int minimalDistanceToFriends(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits);

	};

}
