#pragma once
#include <unordered_map>
#include <Representation/UnitType.h>
#include <Representation/TBSGameState.h>

namespace SGA
{
	using WeightedUnitTypeStrengthLookup = std::unordered_map<int, double>;

	class WeightedUnitTypeEvaluator
	{
	public:
		/// <summary>
		/// Returns a dictionary mapping a unit - type to a type - strength.A higher strength value means the unit is more powerful.
		/// The evaluation uses the fact that the unit with the highest stats is the most powerful.
		/// Based on this, the score is a linear sum of various stats, divided by their maximum possible stat.
		/// 
		/// As an example, if the highest maximum health a unit can have is 400, then all unit - types get a score of maximum_health / 400 added to their strength.
		/// </summary>
		static const WeightedUnitTypeStrengthLookup& getLinearSumEvaluation(const TBSGameState& gameState,
			const double damageMultiplier, const double healthMultiplier, const double healMultiplier,
			const double movementRangeMultiplier, const double actionRangeMultiplier, const double actionsPerTurnMultiplier)
		{
			static auto lookup = computeWeightedSumEvaluation(gameState, damageMultiplier, healthMultiplier,
				healMultiplier, movementRangeMultiplier,  actionRangeMultiplier, actionsPerTurnMultiplier);
			return lookup;
		}

	private:
		static WeightedUnitTypeStrengthLookup computeWeightedSumEvaluation(const TBSGameState& gameState,
			const double damageMultiplier, const double healthMultiplier, const double healMultiplier,
			const double movementRangeMultiplier, const double actionRangeMultiplier, const double actionsPerTurnMultiplier)
		{
			
			WeightedUnitTypeStrengthLookup strengthLookup;
			for (const auto& entry : gameState.getUnitTypes())
			{
				double score = 0;
				// The more different actions a unit can execute, the more its worth
				auto actionRangePerc = entry.second.actionRange * actionRangeMultiplier;
				for (const auto& action : entry.second.actions)
				{
					if (action == ActionType::Move)
						score += entry.second.movementRange * movementRangeMultiplier;
					else if (action == ActionType::Attack)
						score += entry.second.attackDamage * damageMultiplier;
					else if (action == ActionType::Heal)
						score += entry.second.healAmount * healMultiplier;
					else if (action == ActionType::Push)
						score += actionRangePerc;
				}
				// More health = Good
				score += entry.second.maxHealth * healthMultiplier;
				
				// The units worth scales linearly with how many actions it can execute per turn
				double actionBonus = entry.second.actionsPerTurn * actionsPerTurnMultiplier;
				
				// If we can execute same actions multiple times, we treat this as an bonus equivilant to being able to execute an action more
				if (entry.second.canRepeatActions && entry.second.actionsPerTurn > 1)
					actionBonus += actionsPerTurnMultiplier;
				
				strengthLookup[entry.second.id] = score * actionBonus;
			}

			return strengthLookup;
		}
	};
}
