#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class PushThemAllHeuristic : public StateHeuristic
	{
	public:
        double maximum_distance=10000.0;
        double initial_opponent_unit_size = 0;
        double initial_self_unit_size = 0;
        double maximum_sum_distance = 0.0;

        PushThemAllHeuristic();
        PushThemAllHeuristic(int playerID, GameState& gamestate);
		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		std::string getName() const override { return "PushThemAllHeuristic"; }
	};
}
