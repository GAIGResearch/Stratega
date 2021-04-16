#pragma once
#include <optional>
#include <vector>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Path.h>
#include <Stratega/ForwardModel/Action.h>

namespace SGA
{
	/// <summary>
	/// Is an object that interacts with the game and responds to player input or other entities.
	/// This entity can be assigned to a player performing a set of actions per tick/turn.
	/// It has a list of parameters that store real values.
	/// If the player wants to check the <see cref="SGA::EntityType"/> it can search it using the assigned typeID.
	/// </summary>
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

		/// <summary>
		/// Returns a <see cref="SGA::ActionInfo"/> object linked to action type,
		/// used to check when was the last time the action type was executed by this entity
		/// </summary>
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