#pragma once
#include <optional>
#include <vector>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Path.h>
#include <Stratega/ForwardModel/Action.h>

namespace SGA
{
	struct Entity
	{
		Entity():
			typeID(0),
			id(0),
			ownerID(0),
			position(),
		    lineOfSightRange(5),
		    path(),
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
		double lineOfSightRange;

		//RTS Stuff
		Path path;
		double movementSpeed;
		double collisionRadius;

		//Continuous Action
		std::vector<Action> continuousAction;
		bool isNeutral() const;

		const ActionInfo& getActionInfo(int actionTypeID) const
		{
			for (auto& actionInfo : attachedActions)
			{
				if (actionInfo.actionTypeID == actionTypeID)
					return actionInfo;
			}

			throw std::runtime_error("Tried accessing actionInfo of unknown action type");
		}

	};
}