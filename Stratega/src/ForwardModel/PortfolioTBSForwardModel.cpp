#include <ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	std::vector<TBSAction> PortfolioTBSForwardModel::generateActions(TBSGameState& state) const
	{
		auto actionSpace = TBSForwardModel::generateActions(state, state.currentPlayer);

		// filter all the actions according to the portfolio
		std::vector<TBSAction> portfolioActionSpace;

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace.emplace_back(action);
		}
		
		return portfolioActionSpace;
	}

	std::vector<TBSAction> PortfolioTBSForwardModel::generateActions(TBSGameState& state, int playerID) const
	{
		return TBSForwardModel::generateActions(state, playerID);
	}

}
