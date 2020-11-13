#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>
#include <map>


namespace SGA {
	class AttackWeakestOpponentScript : public BaseActionScript
	{

	public:
		AttackWeakestOpponentScript() : BaseActionScript() {};

		TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const override;
		TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "AttackWeakestOpponentScript"; };
	};

}
