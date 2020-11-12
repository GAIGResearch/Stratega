#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>
#include <Agent/ActionScripts/BaseActionScript.h>
#include <set>
#include <map>


namespace SGA {
	class AttackWeakestOpponentScript : public BaseActionScript
	{

	public:
		AttackWeakestOpponentScript() : BaseActionScript() {};

		Action<Vector2i> getAction(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackWeakestOpponentScript"; };
	};

}
