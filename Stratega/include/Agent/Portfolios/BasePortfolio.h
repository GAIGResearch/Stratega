#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>
#include <ForwardModel/TBSActionSpace.h>

namespace SGA {
	class BasePortfolio
	{

	public:
		BasePortfolio() = default;
		virtual ~BasePortfolio() = default;
		
		virtual Action<Vector2i> getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const = 0;
		virtual Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const = 0;
	};
}
