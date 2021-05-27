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

		//Internal state of this entity.
		bool remove = false;
		std::vector<ActionInfo> attachedActions;
		std::vector<double> parameters;
		std::vector<Action> continuousAction;

	public:

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

		//ID of this entity in the game.
		int id;

		//ID of the player who owns this entity.
		int ownerID;

		//ID of the type of entity.
		int typeID;

		//Position of the entity in the board.
		Vector2f position;

		//Range of the line of sight of this entity.
		double lineOfSightRange;

		//Variables for continuous movement of the unit in RTS games.
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


		std::vector<ActionInfo> getAttachedActions() const { return attachedActions; }
		void setActionTicks(int actionTypeID, int tick);

		//Functions for flagging a unit for removal.
		void flagRemove() { remove = true; } 
		bool flagged() { return remove; }
		
		//Initializes the entity with a given ID. Creates default attached actions and parameters list from the entity type.
		void init(int entityID);

		//Prints information about the entity.
		void printInfo() const;

		//Get Parameters list functions.
		std::vector<double>& getParamValues() { return parameters; }
		const std::vector<double>& getParamValues() const { return parameters; }

		//Get specific parameters, by name or index.
		double getParameter(std::string paramName) const;
		double& getParameterAt(int paramIdx) { return parameters[paramIdx]; }

		// Get continuous actions
		std::vector<Action>& getContinuousActions() { return continuousAction; }
		const std::vector<Action>& getContinuousActions() const { return continuousAction; }
	};
}