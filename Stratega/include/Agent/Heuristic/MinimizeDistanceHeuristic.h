#pragma once
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/UnitEvaluator.h>

#include "LinearSumHeuristic.h"

namespace SGA
{
	class MinimizeDistanceHeuristic : public StateHeuristic
	{
	public:
		MinimizeDistanceHeuristic() {};
		MinimizeDistanceHeuristic::~MinimizeDistanceHeuristic() {};
		double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) override;
		static std::string getName() { return "MinimizeDistanceHeuristic"; }
	};
}
