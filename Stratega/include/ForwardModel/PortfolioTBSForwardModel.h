#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/Effect.h>
#include <ForwardModel/FMState.h>
#include <Configuration/WinConditionType.h>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/TBSActionSpace.h>
#include <ForwardModel/TBSForwardModel.h>

#include "Agent/ActionScripts/BaseActionScript.h"

namespace SGA
{
	class PortfolioTBSForwardModel : public TBSForwardModel
	{
	public:
		std::vector<std::unique_ptr<BaseActionScript>>& portfolio;
		
		PortfolioTBSForwardModel(TBSForwardModel forwardModel, std::vector<std::unique_ptr<BaseActionScript>>& portfolio) : TBSForwardModel(std::move(forwardModel)), portfolio(portfolio)
		{
		}
		
		
	protected:

		ActionSpace<Vector2i>* generateActions(TBSGameState& state) const override;
		ActionSpace<Vector2i>* generateActions(TBSGameState& state, int playerID) const override;

	};
}
