#pragma once
#include <unordered_map>
#include <Representation/UnitType.h>

namespace SGA
{
	using UnitTypeStrengthLookup = std::unordered_map<int, double>;

	class UnitTypeEvaluator
	{
	public:
		/// <summary>
		/// Returns a dictionary mapping a unit - type to a type - strength.A higher strength value means the unit is more powerful.
		/// The evaluation uses the fact that the unit with the highest stats is the most powerful.
		/// Based on this, the score is a linear sum of various stats, divided by their maximum possible stat.
		/// 
		/// As an example, if the highest maximum health a unit can have is 400, then all unit - types get a score of maximum_health / 400 added to their strength.
		/// </summary>
		static const UnitTypeStrengthLookup& getLinearSumEvaluation(const TBSGameState& gameState)
		{
			// TODO this does not work
			static auto lookup = computeLinearSumEvaluation(gameState);
			return lookup;
		}

	private:
		static UnitTypeStrengthLookup computeLinearSumEvaluation(const TBSGameState& gameState)
		{
			auto maxDamage = 0;
			auto maxHeal = 0;
			auto maxHealth = 0;
			auto maxMovementRange = 0;
			auto maxActionRange = 0;
			
			for (const auto& entry : gameState.getUnitTypes())
			{
				maxDamage = std::max(entry.second.attackDamage, maxDamage);
				maxHeal = std::max(entry.second.healAmount, maxHeal);
				maxHealth = std::max(entry.second.maxHealth, maxHealth);
				maxMovementRange = std::max(entry.second.movementRange, maxMovementRange);
				maxActionRange = std::max(entry.second.actionRange, maxActionRange);
			}

			UnitTypeStrengthLookup strengthLookup;
			for (const auto& entry : gameState.getUnitTypes())
			{
				double score = 0;
				// The more different actions a unit can execute, the more its worth
				auto actionRangePerc = entry.second.actionRange / static_cast<double>(maxActionRange);
				for (const auto& action : entry.second.tbsActions)
				{
					if (action == TBSActionType::Move)
						score += entry.second.movementRange / static_cast<double>(maxMovementRange);
					else if (action == TBSActionType::Attack)
						score += (entry.second.attackDamage / static_cast<double>(maxDamage)) * actionRangePerc;
					else if (action == TBSActionType::Heal)
						score += (entry.second.healAmount / static_cast<double>(maxHeal)) * actionRangePerc;
					else if (action == TBSActionType::Push)
						score += actionRangePerc;
				}
				// More health = Good
				score += entry.second.maxHealth / static_cast<double>(maxHealth);

				// The units worth scales linearly with how many actions it can execute per turn
				double actionBonus = entry.second.actionsPerTurn;
				
				// If we can execute same actions multiple times, we treat this as an bonus equivilant to being able to execute an action more
				if (entry.second.canRepeatActions && entry.second.actionsPerTurn > 1)
					actionBonus += 1;
				
				strengthLookup[entry.second.id] = score * actionBonus;
			}

			return strengthLookup;
		}
	};
}
