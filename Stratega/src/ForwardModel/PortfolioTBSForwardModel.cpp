#include <ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	std::vector<Action> PortfolioTBSForwardModel::generateActions(TBSGameState2& state) const
	{
		auto actionSpace = TBSAbstractForwardModel::generateActions(state, state.currentPlayer);

		// filter all the actions according to the portfolio
		std::vector<Action> portfolioActionSpace;

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace.emplace_back(action);
		}
		
		return portfolioActionSpace;
	}

	std::vector<Action> PortfolioTBSForwardModel::generateActions(TBSGameState2& state, int playerID) const
	{
		return TBSAbstractForwardModel::generateActions(state, playerID);
	}

}
