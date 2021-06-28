#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

namespace SGA
{
	class DFSAgent : public Agent
	{

	public:
		MinimizeDistanceHeuristic _stateHeuristic;
		int maxDepth = 3;
		int forwardModelCalls = 2000;
		int remainingForwardModelCalls = forwardModelCalls;
		std::unique_ptr<BaseActionScript> opponentModel = std::make_unique<RandomActionScript>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		DFSAgent() :
			Agent{},
			_stateHeuristic()
		{
		}

		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, long timeBudgetMs) override;

		double evaluateRollout(const TBSForwardModel& forwardModel, GameState& gameState, int depth, int playerID);
		void applyActionToGameState(const TBSForwardModel& forwardModel, GameState& gameState, const Action& action);

	};
}
