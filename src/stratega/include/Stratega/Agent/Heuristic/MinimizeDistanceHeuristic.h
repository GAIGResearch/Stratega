#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class MinimizeDistanceHeuristic : public StateHeuristic
	{
	public:
        double maximum_distance=10000.0;
        MinimizeDistanceHeuristic();
        MinimizeDistanceHeuristic(GameState& gamestate);
		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		std::string getName() const override { return "MinimizeDistanceHeuristic"; }
	};
}
