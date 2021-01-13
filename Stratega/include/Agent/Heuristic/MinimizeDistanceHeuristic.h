#pragma once
#include <Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class MinimizeDistanceHeuristic : public StateHeuristic
	{
	public:
		double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) override;
		static std::string getName() { return "MinimizeDistanceHeuristic"; }
	};
}
