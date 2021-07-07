#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/MCTSNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>

#include "MCTSParameters.h"

namespace SGA
{
	
	class ActionAbstractionMCTSAgent : public Agent
	{
	public:
		explicit ActionAbstractionMCTSAgent(MCTSParameters&& params)
			: parameters_(std::move(params))
		{
			// Scripts={AC AW }
			parameters_.portfolio.clear();
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			parameters_.portfolio.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			parameters_.portfolio.emplace_back(std::move(attackWeak));
		}


		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;
		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;

	private:
		bool initialized = false;
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
