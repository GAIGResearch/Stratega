#pragma once
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/WeightedUnitEvaluator.h>

namespace SGA {
	class WeightedSumHeuristic :
		public StateHeuristic
	{
	private:
		int playerID;
		int numberOfPlayers;
		int damageMultiplier;
		int healthMultiplier;
		int healMultiplier;
		int movementRangeMultiplier;
		int actionRangeMultiplier;
		int actionsPerTurnMultiplier;
		
		std::vector<float> weightVector;

	protected:


	public:
		WeightedSumHeuristic(const int playerID, const int numberOfPlayers, const double damageMultiplier, const double healthMultiplier,
			const double healMultiplier, const double movementRangeMultiplier, const double actionRangeMultiplier, const double actionsPerTurnMultiplier);
		~WeightedSumHeuristic();

		double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) override;

		double getOwnScore(TBSGameState* gsOld, TBSGameState* gsNew);

		double score(TBSGameState* gsOld, TBSGameState* gsNew, int playerId);
	};

}

