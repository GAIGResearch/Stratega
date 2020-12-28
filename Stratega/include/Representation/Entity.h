#pragma once
#include <vector>
#include <Representation/Vector2.h>
#include <Representation/Path.h>
#include <ForwardModel/Action.h>

namespace SGA
{
	struct Entity
	{
		Entity():
			typeID(0),
			id(0),
			ownerID(0),
			position(),
			canExecuteAction(true),
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
		std::vector<int> actionTypeIds;

		std::vector<double> parameters;

		bool shouldRemove=false;//Death?

		//TODO depends on the type...
		bool canExecuteAction;

		//RTS Stuff
		Action intendedAction;
		Action executingAction;
		Path path;
		double actionCooldown;
		double maxActionCooldown;
		double movementSpeed;
		float collisionRadius;
	};
}