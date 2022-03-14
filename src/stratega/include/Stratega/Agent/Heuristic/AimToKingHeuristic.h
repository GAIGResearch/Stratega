#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>


namespace SGA
{
	class AimToKingHeuristic : public StateHeuristic
	{
	private:
		int unitNum = -1;
	public:
        AimToKingHeuristic(GameState& gameState);

		double evaluateGameState(
           const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;

		std::string getName() const override { return "AimToKingHeuristic"; }
	};
        
}
