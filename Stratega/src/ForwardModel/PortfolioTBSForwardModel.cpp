#include <ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	std::vector<SGA::Action<Vector2i>>* PortfolioTBSForwardModel::generateActions(TBSGameState& state) const
	{
		auto actionSpace = std::unique_ptr<std::vector<SGA::Action<Vector2i>>>(TBSForwardModel::generateActions(state, state.currentPlayer));

		// filter all the actions according to the portfolio
		auto* portfolioActionSpace = new std::vector<SGA::Action<Vector2i>>();

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace->emplace_back(action);
		}
		
		return portfolioActionSpace;
	}

	std::vector<SGA::Action<Vector2i>>* PortfolioTBSForwardModel::generateActions(TBSGameState& state, int playerID) const
	{
		return TBSForwardModel::generateActions(state, playerID);
	}

}
