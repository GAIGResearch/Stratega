#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class PortfolioTBSForwardModel : public TBSForwardModel
	{
	public:
		std::vector<std::shared_ptr<BaseActionScript>>& portfolio;
		
		PortfolioTBSForwardModel(TBSForwardModel forwardModel, std::vector<std::shared_ptr<BaseActionScript>>& portfolio)
			: TBSForwardModel(std::move(forwardModel)), portfolio(portfolio) {}
		void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const override;
	};

	
	class PortfolioRTSForwardModel : public RTSForwardModel
	{
	public:
		std::vector<std::shared_ptr<BaseActionScript>>& portfolio;

		PortfolioRTSForwardModel(RTSForwardModel forwardModel, std::vector<std::shared_ptr<BaseActionScript>>& portfolio)
			: RTSForwardModel(std::move(forwardModel)), portfolio(portfolio) {}
		void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const override;
	};


}
