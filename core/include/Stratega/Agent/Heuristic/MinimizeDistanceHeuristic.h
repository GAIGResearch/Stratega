#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class MinimizeDistanceHeuristic : public StateHeuristic
	{
	public:
		double evaluateGameState(const TBSForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		static std::string getName() { return "MinimizeDistanceHeuristic"; }
	};
}
