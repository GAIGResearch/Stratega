#include <Stratega/ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	std::vector<Action> PortfolioTBSForwardModel::generateActions(GameState& state) const
	{
		auto actionSpace = TBSForwardModel::generateActions(state, state.currentPlayer);

		// filter all the actions according to the portfolio
		std::vector<Action> portfolioActionSpace;

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace.emplace_back(action);
		}
		
		return portfolioActionSpace;
	}

	std::vector<Action> PortfolioTBSForwardModel::generateActions(GameState& state, int playerID) const
	{
		return TBSForwardModel::generateActions(state, playerID);
	}

}
