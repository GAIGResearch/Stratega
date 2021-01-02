#pragma once
#include <optional>
#include <vector>
#include <Representation/Vector2.h>
#include <Representation/Path.h>
#include <ForwardModel/Action.h>

namespace SGA
{
	struct Entity
	{
		struct ActionInfo
		{
			int actionTypeID;
			int lastExecutedTick;
		};
		
		Entity():
			typeID(0),
			id(0),
			ownerID(0),
			position(),
		    intendedAction(),
		    executingAction(),
		    path(),
		    actionCooldown(0),
		    maxActionCooldown(1),
		    movementSpeed(2),
		    collisionRadius(0.5)
		{
		}

		int typeID;
		int id;
		int ownerID;
		Vector2f position;
		std::vector<ActionInfo> attachedActions;
		std::vector<double> parameters;
		bool shouldRemove=false;

		//RTS Stuff
		std::optional<Action> intendedAction;
		std::optional<Action> executingAction;
		Path path;
		double actionCooldown;
		double maxActionCooldown;
		double movementSpeed;
		float collisionRadius;
	};
}