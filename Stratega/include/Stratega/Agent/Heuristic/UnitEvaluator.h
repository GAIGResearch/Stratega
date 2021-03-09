#pragma once
#include <unordered_map>
#include <Stratega/Representation/EntityType.h>

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
		static const UnitTypeStrengthLookup& getLinearSumEvaluation(const GameState& gameState)
		{
			// TODO this does not work
			static auto lookup = computeLinearSumEvaluation(gameState);
			return lookup;
		}

	private:
		static constexpr unsigned int str2int(const char* str, int h = 0)
		{
			return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
		}
		
		static UnitTypeStrengthLookup computeLinearSumEvaluation(const GameState& gameState)
		{
			double maxDamage = 0;
			double maxHeal = 0;
			double maxHealth = 0;
			double maxMovementRange = 0;
			double maxActionRange = 0;

			for (const auto& entry : *gameState.entityTypes)
			{
				for (const auto& parameter : entry.second.parameters)
				{
					if (parameter.second.name == "AttackDamage")
					{
						maxDamage = std::max(parameter.second.maxValue, maxDamage);
						continue;
					}
					if (parameter.second.name == "AttackRange" || parameter.second.name == "HealRange")
					{
						maxActionRange = std::max(parameter.second.maxValue, maxActionRange);
						continue;
					}
					if (parameter.second.name == "Health")
					{
						maxHealth = std::max(parameter.second.maxValue, maxHealth);
						continue;
					}
					if (parameter.second.name == "MovementPoints" )
					{
						maxMovementRange = std::max(parameter.second.maxValue, maxMovementRange);
						continue;
					}
					if (parameter.second.name == "HealAmount")
					{
						maxHeal = std::max(parameter.second.maxValue, maxHeal);
						continue;
					}
					else
					{
						std::cout << parameter.second.name;
					}
				}
			}

			
			UnitTypeStrengthLookup strengthLookup;
			
			for (const auto& entry : *gameState.entityTypes)
			{
				double score = 0;
				auto actionRangePerc = 1;
				double attackDamage = 0;
				double healAmount = 0;
				double movementRange = 0;

				for (const auto& parameter : entry.second.parameters)
				{
					if (parameter.second.name == "AttackDamage")
					{
						continue;
					}
					if (parameter.second.name == "AttackRange" || parameter.second.name == "HealRange")
					{
						actionRangePerc = parameter.second.defaultValue / static_cast<double>(maxActionRange);
						continue;
					}
					if (parameter.second.name == "Health")
					{
						score += parameter.second.maxValue / static_cast<double>(maxHealth);
						continue;
					}
					if (parameter.second.name == "MovementPoints")
					{
						movementRange = parameter.second.defaultValue;
						continue;
					}
					if (parameter.second.name == "HealAmount")
					{
						healAmount += parameter.second.defaultValue / static_cast<double>(maxHeal);
						continue;
					}
					else
					{
						std::cout << parameter.second.name;
					}
				}
				
				for (auto actionID : entry.second.actionIds)
				{
					const auto& action = (*gameState.actionTypes)[actionID];
					if (action.name == "Attack")
					{
						score += (attackDamage / static_cast<double>(maxDamage)) * actionRangePerc;
						continue;
					}
					if (action.name == "Move")
					{
						score += movementRange / static_cast<double>(maxMovementRange);
						continue;
					}
					if (action.name == "Heal")
					{
						score += (healAmount / static_cast<double>(maxHeal)) * actionRangePerc;
						continue;
					}
					else
					{
						std::cout << "action unknown" << action.name;
					}
				}
				
				strengthLookup[entry.second.id] = score;
			}
			
			return strengthLookup;
		}
	};
}