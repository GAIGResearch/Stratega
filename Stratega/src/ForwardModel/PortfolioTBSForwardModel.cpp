#include <ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	std::vector<Action<Vector2i>> PortfolioTBSForwardModel::getActions(TBSGameState& state) const
	{
		auto actionSpace = TBSForwardModel::getActions(state, state.currentPlayer);

		// filter all the actions according to the portfolio
		std::vector<Action<Vector2i>> portfolioActionSpace;

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace.emplace_back(action);
		}
		
		return portfolioActionSpace;
	}

	std::vector<Action<Vector2i>> PortfolioTBSForwardModel::getActions(TBSGameState& state, int playerID) const
	{
		return TBSForwardModel::getActions(state, playerID);
	}

}
