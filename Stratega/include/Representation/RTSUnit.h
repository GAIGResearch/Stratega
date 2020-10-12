#pragma once
#include <Representation/Vector2.h>
#include <Representation/RTSGameState.h>
#include <ForwardModel/ActionType.h>
#include <ForwardModel/Action.h>


namespace SGA
{
	class RTSGameState;
	
	class RTSUnit
	{
	public:
		RTSUnit(RTSGameState& state, int unitId, int playerId) :
			playerID(playerId),
			unitID(unitId),
			unitTypeID(0),
			executingAction(nullptr),
			actionRange(0),
			actionCooldown(0),
			maxActionCooldown(1),
			movementSpeed(2),
			collisionRadius(0.5),
			position(),
			health(40),
			attackDamage(20),
			healAmount(10),
			state(state)
		{
		}

		RTSUnit(const RTSUnit& unit) = default;

		int playerID;
		int unitID;
		int unitTypeID;

		std::shared_ptr<Action<Vector2f>> executingAction;
		double actionRange;
		double actionCooldown;
		double maxActionCooldown;
		double movementSpeed;
		float collisionRadius;
		
		Vector2f position;
		int health;
		int attackDamage;
		int healAmount;
		std::reference_wrapper<RTSGameState> state;
	};
}
