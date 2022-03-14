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
			return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ static_cast<unsigned int>(str[static_cast<size_t>(h)]);
		}

		static UnitTypeStrengthLookup computeLinearSumEvaluation(const GameState& gameState)
		{
			double maxDamage = 0;
			double maxHeal = 0;
			double maxHealth = 0;
			double maxMovementRange = 0;
			double maxActionRange = 0;

			for (const auto& entry : gameState.getGameInfo()->getEntityTypes())
			{
				for (const auto& parameter : entry.second.getParameters())
				{
					if (parameter.second.getName() == "AttackDamage")
					{
						maxDamage = std::max(parameter.second.getMaxValue(), maxDamage);
						continue;
					}
					if (parameter.second.getName() == "AttackRange" || parameter.second.getName() == "HealRange")
					{
						maxActionRange = std::max(parameter.second.getMaxValue(), maxActionRange);
						continue;
					}
					if (parameter.second.getName() == "Health")
					{
						maxHealth = std::max(parameter.second.getMaxValue(), maxHealth);
						continue;
					}
					if (parameter.second.getName() == "MovementPoints")
					{
						maxMovementRange = std::max(parameter.second.getMaxValue(), maxMovementRange);
						continue;
					}
					if (parameter.second.getName() == "HealAmount")
					{
						maxHeal = std::max(parameter.second.getMaxValue(), maxHeal);
						continue;
					}
					else
					{
						std::cout << parameter.second.getName();
					}
				}
			}


			UnitTypeStrengthLookup strengthLookup;
			for (const auto& entry : gameState.getGameInfo()->getEntityTypes())
			{
				double score = 0;
				auto actionRangePerc = 1;
				double attackDamage = 0;
				double healAmount = 0;
				double movementRange = 0;

				for (const auto& parameter : entry.second.getParameters())
				{
					if (parameter.second.getName() == "AttackDamage")
					{
						continue;
					}
					if (parameter.second.getName() == "AttackRange" || parameter.second.getName() == "HealRange")
					{
						actionRangePerc = static_cast<int>(parameter.second.getDefaultValue() / maxActionRange);
						continue;
					}
					if (parameter.second.getName() == "Health")
					{
						score += parameter.second.getMaxValue() / maxHealth;
						continue;
					}
					if (parameter.second.getName() == "MovementPoints")
					{
						movementRange = parameter.second.getDefaultValue();
						continue;
					}
					if (parameter.second.getName() == "HealAmount")
					{
						healAmount += parameter.second.getDefaultValue() / maxHeal;
						continue;
					}
					else
					{
						std::cout << parameter.second.getName();
					}
				}

				for (auto actionID : entry.second.getActionIDs())
				{
					const auto& action = (gameState.getGameInfo()->getActionTypes()).at((actionID));
					if (action.getName() == "Attack")
					{
						score += (attackDamage / maxDamage) * actionRangePerc;
						continue;
					}
					if (action.getName() == "Move")
					{
						score += movementRange / maxMovementRange;
						continue;
					}
					if (action.getName() == "Heal")
					{
						score += (healAmount / maxHeal) * static_cast<double>(actionRangePerc);
						continue;
					}
					if (action.getName() == "Empty")
					{ }
					else
					{
						std::cout << "action unknown: " << action.getName() << std::endl;
					}
				}

				strengthLookup[entry.second.getID()] = score;
			}

			return strengthLookup;
		}
	};
}