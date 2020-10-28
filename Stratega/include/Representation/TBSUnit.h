#pragma once
#include <Representation/Vector2.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/ActionType.h>
#include <Representation/UnitType.h>

namespace SGA
{
	class TBSGameState;

	class  TBSUnit
	{
	public:
		std::unordered_set<ActionType> executedActionTypes;
		int numActionsExecuted;
		
		TBSUnit(TBSGameState& state, int unitId, int playerId):
			numActionsExecuted(0),
			state(state),
			unitTypeID(0),
			unitID(unitId),
			playerID(playerId),
			position(),
			health(40),
			maxHealth(health),
			attackDamage(20),
			healAmount(10),
			range(5),
			lineOfSightRange(6)
		{
		}

		int getUnitTypeID() const { return unitTypeID; }
		int getUnitID() const;
		int getPlayerID() const;
		const Vector2i& getPosition() const;
		void setTypeID(int newTypeID) { unitTypeID = newTypeID; };
		void setPosition(Vector2i pos) { position = pos; }
		void setHealth(int newHealth) { health = newHealth; }
		void setMaxHealth(int newHealth) { maxHealth = newHealth; }
		void setAttackDamage(int newAttackDamage) { attackDamage = newAttackDamage; }
		void setHealAmount(int newHealAmount) { healAmount = newHealAmount; }
		void setRange(int newRange) { range = newRange; }
		void setLineOfSightRange(int newLineOfSightRange) { lineOfSightRange = newLineOfSightRange; }

		const UnitType& getType() const;
		int& getHealth();
		int getHealthh()const  { return health; }
		int getMaxHealth()const  { return maxHealth; }
		int getAttackDamage() const;
		int getHealAmount() const;
		int getRange() const;
		int getLineOfSightRange() const;
		
		bool isDead() const;

		bool operator==(const TBSUnit& other) const;
		
		std::reference_wrapper<TBSGameState> state;
	private:
		int unitTypeID;
		int unitID;
		int playerID;
		Vector2i position;

		int health;
		int maxHealth;
		int attackDamage;
		int healAmount;
		int range;
		int lineOfSightRange;
	};
}