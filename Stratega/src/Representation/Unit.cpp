#include <Representation/TBSUnit.h>

namespace SGA
{	
	bool TBSUnit::operator==(const TBSUnit& other) const
	{
		return unitID == other.unitID;
	}
	
	int TBSUnit::getUnitID() const
	{
		return unitID;
	}
	
	int TBSUnit::getPlayerID() const
	{
		return playerID;
	}

	const UnitType& TBSUnit::getType() const
	{
		return state.get().getUnitTypes().at(unitTypeID);
	}

	const Vector2i& TBSUnit::getPosition() const
	{
		return position;
	}

	int& TBSUnit::getHealth()
	{
		return health;
	}

	int TBSUnit::getAttackDamage() const
	{
		return attackDamage;
	}

	int TBSUnit::getHealAmount() const
	{
		return healAmount;
	}

	int TBSUnit::getRange() const
	{
		return range;
	}

	int TBSUnit::getLineOfSightRange() const
	{
		return lineOfSightRange;
	}

	bool TBSUnit::isDead() const
	{
		if (health <= 0)
			return true;
		else
			return false;
	}
}
