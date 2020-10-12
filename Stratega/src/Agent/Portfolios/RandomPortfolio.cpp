#include <Agent/Portfolios/RandomPortfolio.h>

namespace SGA
{
	Action<Vector2i> RandomPortfolio::getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const
	{
		return actionSpace->getAction(rand() % actionSpace->count());
	}
	
	Action<Vector2i> RandomPortfolio::getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const
	{
		std::vector<Action<Vector2i>> suitableActions = std::vector<Action<Vector2i>>();
		for (const auto& action : *actionSpace)
		{
			if (action.getSourceUnitID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
			return suitableActions.at(rand() % suitableActions.size());
		
		return actionSpace->getAction(rand() % actionSpace->count());
	}
}
