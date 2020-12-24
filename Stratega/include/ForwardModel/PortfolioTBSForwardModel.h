#pragma once
#include <Representation/AbstractGS/TBSGameState2.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include "Agent/ActionScripts/BaseActionScript.h"

namespace SGA
{
	class PortfolioTBSForwardModel : public TBSAbstractForwardModel
	{
	public:
		std::vector<std::unique_ptr<BaseActionScript>>& portfolio;
		
		PortfolioTBSForwardModel(TBSAbstractForwardModel forwardModel, std::vector<std::unique_ptr<BaseActionScript>>& portfolio)
			: TBSAbstractForwardModel(std::move(forwardModel)), portfolio(portfolio)
		{
		}
		
		std::vector<Action> generateActions(TBSGameState2& state) const override;
		std::vector<Action> generateActions(TBSGameState2& state, int playerID) const override;

	};
}
