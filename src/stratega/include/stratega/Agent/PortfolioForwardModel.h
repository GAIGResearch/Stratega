#pragma once
#include <stratega/Representation/GameState.h>
#include <stratega/ForwardModel/TBSForwardModel.h>
#include <stratega/ForwardModel/RTSForwardModel.h>
#include <stratega/Agent/ActionScripts/BaseActionScript.h>

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
