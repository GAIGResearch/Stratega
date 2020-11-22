#include <Configuration/UnitConfig.h>

namespace SGA
{
	UnitType unitTypeFromConfig(const UnitConfig& config, int id)
	{
		UnitType type;
		type.id = id;
		type.attackDamage = config.attackDamage;
		type.healAmount = config.healAmount;
		type.lineOfSightRange = config.lineOfSightRange;
		type.maxHealth = config.health;
		type.tbsActions = config.tbsActions;
		type.movementRange = config.movementRange;
		type.actionRange = config.actionRange;
		type.canRepeatActions = config.canRepeatActions;
		type.actionsPerTurn = config.actionsPerTurn;
		type.collisionRadius = config.collisionRadius;
		return type;
	}
}