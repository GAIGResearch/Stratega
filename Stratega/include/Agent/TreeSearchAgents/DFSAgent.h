#pragma once
#include <Agent/Agent.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>


#include "Agent/Portfolios/AttackClosest.h"
#include "Agent/Portfolios/BasePortfolio.h"

namespace SGA
{
	class DFSAgent : public Agent
	{

	public:
		LinearSumHeuristic _stateHeuristic;
		int maxDepth = 3;
		int forwardModelCalls = 2000;
		int remainingForwardModelCalls = forwardModelCalls;
		std::unique_ptr<BasePortfolio> opponentModel = std::make_unique<AttackClosest>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		DFSAgent() :
			Agent{}, _stateHeuristic(LinearSumHeuristic())
		{

		}
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

		double evaluateRollout(TBSForwardModel& forwardModel, TBSGameState& gameState, int depth, const int playerID);
		void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const Action<Vector2i>& action);

	};
}
