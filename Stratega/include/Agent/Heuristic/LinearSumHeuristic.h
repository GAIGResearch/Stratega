#pragma once
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/UnitEvaluator.h>

namespace SGA
{
	class LinearSumHeuristic : public StateHeuristic
	{
	public:
		LinearSumHeuristic() {};
		~LinearSumHeuristic() {};
		double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) override;
		static std::string getName() { return "LinearSumHeuristic"; }
	};
}
