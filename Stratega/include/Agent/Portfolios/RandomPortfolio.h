#pragma once
#include <Agent/Portfolios/BasePortfolio.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>


namespace SGA {
	class RandomPortfolio : public BasePortfolio
	{

	public:
		RandomPortfolio() : BasePortfolio() {};
		//~RandomPortfolio() override {};

		Action<Vector2i> getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const override;
	};
}
