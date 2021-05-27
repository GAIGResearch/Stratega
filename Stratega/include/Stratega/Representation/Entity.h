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

	public:

		/// <summary>
		//ID of this entity in the game.
		/// <summary>
		int id;

		/// <summary>
		//ID of the player who owns this entity.
		/// <summary>
		int ownerID;

		/// <summary>
		//ID of the type of entity.
		/// <summary>
		int typeID;

		/// <summary>
		//Position of the entity in the board.
		/// <summary>
		Vector2f position;

		/// <summary>
		//Range of the line of sight of this entity.
		/// <summary>
		double lineOfSightRange;

		/* Variables for continuous movement of the unit in RTS games. */

		/// <summary>
		/// Path that this entity is following (RTS)
		/// <summary>
		Path path;

		/// <summary>
		/// Movement speed for this entity (RTS)
		/// <summary>
		double movementSpeed;
		
		/// <summary>
		/// Collision radius for this entity (RTS)
		/// <summary>
		double collisionRadius;

	private:

		//Reference to the Entity type of this entity.
		const EntityType* type;

		//Internal state of this entity.
		bool remove = false;
		std::vector<ActionInfo> attachedActions;
		std::vector<double> parameters;
		std::vector<Action> continuousAction;


	public:

		/// <summary>
		/// Creates an instance of an entity. 
		/// <summary>
		/// <param name="type">Receives a pointer to the type of entity this will be.</param>
		Entity(const EntityType* type) :
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

		/// <summary>
		/// Indicates if this unit is neutral (i.e. does not belong to any playing faction).
		/// <summary>
		/// <returns>True if this entity is neutral.</returns>
		bool isNeutral() const;

		/// <summary>
		/// Returns a <see cref="SGA::ActionInfo"/> object linked to action type,
		/// used to check when was the last time the action type was executed by this entity
		/// </summary>
		/// <param name="actionTypeID">The ID of the action type we're looking for.</param>
		/// <returns>The action info object, if it exists. Throws a runtime error if it doesn't.</returns>
		const ActionInfo& getActionInfo(int actionTypeID) const;


		/// <summary>
		/// Returns the actions attached to this entity.
		/// <summary>
		/// <returns>The list of <see cref="SGA::ActionInfo"/> attached to this entity.</returns>
		std::vector<ActionInfo> getAttachedActions() const { return attachedActions; }


		/// <summary>
		/// Sets the last executed game ticks to the tick passed by parameter for the action corresponding to the type received.
		/// <summary>
		/// <param name="actionTypeID">The ID of the action type we're looking for.</param>
		/// <param name="tick">The value of the tick to set in the action information</param>
		void setActionTicks(int actionTypeID, int tick);

		/// <summary>
		// Flags this entity to be removed by the game engine.
		/// <summary>
		void flagRemove() { remove = true; }
		
		/// <summary>
		/// Checks if this entity should be removed by the game engine.
		/// <summary>
		/// <returns>True if this entity is marked to be removed.</returns>
		bool flagged() { return remove; }

		/// <summary>
		//Initializes the entity with a given ID. Creates default attached actions and parameters list from the entity type.
		/// <summary>
		/// <param name="entityID">(Unique) ID of this entity in the game.</param>
		void init(int entityID);

		/// <summary>
		// Prints information about the entity, parameters and actions
		/// <summary>
		void printInfo() const;

		/// <summary>
		/// Gets the list of parameters values. Modifiable.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		std::vector<double>& getParamValues() { return parameters; }
		
		/// <summary>
		/// Gets the list of parameters values.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		const std::vector<double>& getParamValues() const { return parameters; }

		/// <summary>
		/// Gets the value of a specific parameter, by name 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double getParameter(std::string paramName) const;
		
		/// <summary>
		/// Gets a specific parameters, by index 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double& getParameterAt(int paramIdx) { return parameters[paramIdx]; }

		/// <summary>
		/// Gets the list of continuous actions attached to this entity. Modifiable.
		/// <summary>
		/// <returns>The list of continuous actions attached to this entity.</returns>
		std::vector<Action>& getContinuousActions() { return continuousAction; }

		/// <summary>
		/// Gets the list of continuous actions attached to this entity. 
		/// <summary>
		/// <returns>The list of continuous actions attached to this entity.</returns>
		const std::vector<Action>& getContinuousActions() const { return continuousAction; }
	};
}