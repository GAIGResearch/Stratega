#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class MinimizeDistanceHeuristic : public StateHeuristic
	{
	public:
		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		std::string getName() const override { return "MinimizeDistanceHeuristic"; }
	};
}
