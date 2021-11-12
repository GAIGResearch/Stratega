#include <Stratega/Agent/PortfolioForwardModel.h>

namespace SGA
{

	void PortfolioTBSForwardModel::generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const
	{
		auto actions = actionSpace->generateActions(state, playerID);
		for (std::shared_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			actionBucket.emplace_back(actionScript->getAction(state, actions, playerID));
		}
	}
	
	void PortfolioRTSForwardModel::generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const
	{
		auto actions = actionSpace->generateActions(state, playerID);
		for (std::shared_ptr<BaseActionScript>& actionScript : this->portfolio)
		{
			actionBucket.emplace_back(actionScript->getAction(state, actions, playerID));
		}
	}


}
