#include <Stratega/ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	void PortfolioTBSForwardModel::generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const
	{
		TBSForwardModel::generateActions(state, playerID, actionBucket);
		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actionBucket);
			actionBucket.emplace_back(action);
		}
	}
}
