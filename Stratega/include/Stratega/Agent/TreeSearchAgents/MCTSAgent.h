#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/MCTSNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include "MCTSParameters.h"

namespace SGA
{
	
	class MCTSAgent : public Agent
	{
	public:
		explicit MCTSAgent(MCTSParameters&& params)
			: parameters_(std::move(params))
		{
			//K=0.1, RL=1, OS=3;
			parameters_.K = 0.1;
			parameters_.ROLLOUT_LENGTH = 1;
			//parameters_.OPPONENT_MODEL = nullptr;		// the opponent model has not been correctly set in the NTBEA evaluation
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
