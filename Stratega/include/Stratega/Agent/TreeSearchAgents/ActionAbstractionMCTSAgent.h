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
			//K=10, RL=5, OS=3, 
			parameters_.K = 10;
			parameters_.ROLLOUT_LENGTH = 5;
			//parameters_.OPPONENT_MODEL = nullptr;	// the opponent model has not been correctly set in the NTBEA evaluation

			// Scripts={AC AW }
			parameters_.PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			parameters_.PORTFOLIO.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			parameters_.PORTFOLIO.emplace_back(std::move(attackWeak));
		}


		ActionAssignment computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;

	private:
		bool initialized = false;
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
