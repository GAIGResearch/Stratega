#include <Agent/Heuristic/WeightedSumHeuristic.h>

namespace SGA
{

	double WeightedSumHeuristic::evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState)
	{
		double score = 0.0;
		auto unitScores = WeightedUnitTypeEvaluator::getLinearSumEvaluation(gameState, damageMultiplier, healthMultiplier,
			healMultiplier, movementRangeMultiplier, actionRangeMultiplier, actionsPerTurnMultiplier);
		for (const TBSUnit& unit : gameState.getUnits())
		{
			const int multiplier = unit.getPlayerID() == gameState.currentPlayer ? 1 : -1;
			const double healthRatio = unit.getHealthh() / static_cast<double>(unit.getType().maxHealth);

			score += unitScores[unit.getType().id] * multiplier * healthRatio;
			score += multiplier * healthRatio;
		}

		return score;
	}
}
