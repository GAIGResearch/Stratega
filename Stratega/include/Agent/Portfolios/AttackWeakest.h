#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>
#include <Agent/Portfolios/BasePortfolio.h>
#include <set>
#include <map>


namespace SGA {
	class AttackWeakest : public BasePortfolio
	{

	public:
		AttackWeakest() : BasePortfolio() {};
		~AttackWeakest() override = default;

		Action<Vector2i> getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const override;

	};

}
