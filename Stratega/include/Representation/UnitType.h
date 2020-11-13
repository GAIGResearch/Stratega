#pragma once
#include <string>
#include <unordered_set>
#include <ForwardModel/ActionType.h>
#include <ForwardModel/TBSActionType.h>

namespace SGA
{
	struct UnitType
	{
		int id;
		int maxHealth;
		int attackDamage;
		int healAmount;

		int actionRange;
		int movementRange;
		int lineOfSightRange;

		float collisionRadius;
		
		std::unordered_set<ActionType> actions;
		std::unordered_set<TBSActionType> tbsActions;
		bool canRepeatActions;
		int actionsPerTurn;
	};
}
