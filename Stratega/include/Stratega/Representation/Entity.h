#pragma once
#include <optional>
#include <vector>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Path.h>
#include <Stratega/ForwardModel/Action.h>

namespace SGA
{
	struct GameInfo;

	/// <summary>
	/// Is an object that interacts with the game and responds to player input or other entities.
	/// This entity can be assigned to a player performing a set of actions per tick/turn.
	/// It has a list of parameters that store real values.
	/// If the player wants to check the <see cref="SGA::EntityType"/> it can search it using the assigned typeID.
	/// </summary>
	struct Entity
	{

	private:

		//Reference to the Entity type of this entity.
		const EntityType* type;

		std::vector<double> parameters;
		std::vector<ActionInfo> attachedActions;
		bool remove = false;

	public:
		std::vector<Action> continuousAction;

		Entity(const EntityType* type):
			type(type),
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

		int id;
		int ownerID;
		int typeID;
		Vector2f position;
		double lineOfSightRange;

		//RTS Stuff
		Path path;
		double movementSpeed;
		double collisionRadius;

		//Continuous Action
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

		double getParameter(std::string paramName) const;
		double& getParameterAt(int paramIdx) { return parameters[paramIdx]; }	
		std::vector<double> getParamValues() { return parameters; }
		std::vector<double> getParamValues() const { return parameters; }
		std::vector<ActionInfo> getAttachedActions() { return attachedActions; }
		std::vector<ActionInfo> getAttachedActions() const { return attachedActions; }
		//std::vector<Action> getContinuousActions() { return continuousAction; }
		//std::vector<Action> getContinuousActions() const { return continuousAction; }
		void flagRemove() { remove = true; } //Not sure we'll need to set to false externally ever.
		bool flagged() { return remove; }

		void init(int entityID);
		void printInfo() const;
	};
}