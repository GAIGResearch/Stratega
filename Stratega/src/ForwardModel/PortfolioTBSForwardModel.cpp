#include <ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	ActionSpace<Vector2i>* PortfolioTBSForwardModel::generateActions(TBSGameState& state) const
	{
		auto actionSpace = std::unique_ptr<ActionSpace<Vector2i>>(TBSForwardModel::generateActions(state, state.currentPlayer));

		// filter all the actions according to the portfolio
		auto* portfolioActionSpace = new ActionSpace<Vector2i>();

		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionSpace);
			portfolioActionSpace->addAction(action);
		}
		
		return portfolioActionSpace;
	}

	ActionSpace<Vector2i>* PortfolioTBSForwardModel::generateActions(TBSGameState& state, int playerID) const
	{
		return TBSForwardModel::generateActions(state, playerID);
	}

}
