#include <Stratega/ForwardModel/PortfolioTBSForwardModel.h>

namespace SGA
{
	void PortfolioTBSForwardModel::generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const
	{
		auto actions = actionSpace->generateActions(state, playerID); //TBSForwardModel::generateActions(state, playerID);
		for (std::unique_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			const auto action = actionScript->getAction(state, actions);
			actionBucket.emplace_back(action);
		}
	}
}
