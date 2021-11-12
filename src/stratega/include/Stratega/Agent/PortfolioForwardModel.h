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
		
		PortfolioTBSForwardModel(TBSForwardModel newForwardModel, std::vector<std::shared_ptr<BaseActionScript>>& newPortfolio)
			: TBSForwardModel(std::move(newForwardModel)), portfolio(newPortfolio) {}
		void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const override;
	};

	
	class PortfolioRTSForwardModel : public RTSForwardModel
	{
	public:
		std::vector<std::shared_ptr<BaseActionScript>>& portfolio;

		PortfolioRTSForwardModel(RTSForwardModel newForwardModel, std::vector<std::shared_ptr<BaseActionScript>>& newPortfolio)
			: RTSForwardModel(std::move(newForwardModel)), portfolio(newPortfolio) {}
		void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const override;
	};


}
