#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>

namespace SGA
{
	class PortfolioTBSForwardModel : public ForwardModel
	{
	public:
		std::vector<std::unique_ptr<BaseActionScript>>& portfolio;
		
		PortfolioTBSForwardModel(ForwardModel forwardModel, std::vector<std::unique_ptr<BaseActionScript>>& portfolio)
			: ForwardModel(std::move(forwardModel)), portfolio(portfolio)
		{
		}
		
		std::vector<Action> generateActions(GameState& state) const override;
		std::vector<Action> generateActions(GameState& state, int playerID) const override;

	};
}
