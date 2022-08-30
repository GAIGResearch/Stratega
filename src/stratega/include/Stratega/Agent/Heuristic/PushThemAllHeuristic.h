#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class PushThemAllHeuristic : public StateHeuristic
	{
	public:
        double maximum_distance=10000.0;
        int initial_opponent_unit_size = 0;
        int initial_self_unit_size = 0;

        PushThemAllHeuristic();
        PushThemAllHeuristic(int playerID, GameState& gamestate);
		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
		std::string getName() const override { return "PushThemAllHeuristic"; }
	};
}
