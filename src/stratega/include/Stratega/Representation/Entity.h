#pragma once
//#include <optional>
#include <Stratega/Utils/optional.hpp>
#include <vector>
#include <unordered_map>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Path.h>
#include <Stratega/Representation/Buff.h>
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

		/// <summary>
		//ID of this entity in the game.
		/// <summary>
		int id;

		/// <summary>
		//ID of the player who owns this entity.
		/// <summary>
		int ownerID;

		/// <summary>
		//Position of the entity in the board.
		/// <summary>
		Vector2f position;

		/// <summary>
		//Range of the line of sight of this entity.
		/// <summary>
		double lineOfSightRange;

		/// <summary>
		/// Flag that indicates if this entity will be removed from the game.
		/// </summary>
		bool remove = false;
		
		/// <summary>
		/// Actions that this entity can execute in this game.
		/// </summary>
		std::vector<ActionInfo> attachedActions;

		///// <summary>
		///// Actions that this entity can execute in this game.
		///// </summary>
		//std::vector<ActionInfo> attachedActions;

		/// <summary>
		/// Array of actions currently being executed by this entity
		/// </summary>
		std::vector<Action> continuousAction;

		/// <summary>
		/// Array of buffs currently applied to this entity
		/// </summary>
		std::vector<Buff> buffs;

		/// <summary>
		/// Values for the parameters of this entity. Indexed by ID. Use getParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> parameters;

		/// <summary>
		/// Values for the max parameters value of this entity. Indexed by ID. Use getMaxParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> maxParameters;

		/// <summary>
		/// Values for the min parameters value of this entity. Indexed by ID. Use getMinParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> minParameters;
		
		/// <summary>
		/// Entity type
		/// <summary>
		const EntityType* type;

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

	public:

		/// <summary>
		/// Creates an instance of an entity. 
		/// <summary>

		Entity() :
			id(0),
			ownerID(0),
			position(),
			lineOfSightRange(5),
			type(0),
			path(),
			movementSpeed(2),
			collisionRadius(0.5)
		{
		}

		/// <summary>
		//Initializes the entity with a given ID. Creates default attached actions and parameters list from the entity type.
		/// <summary>
		/// <param name="entityID">(Unique) ID of this entity in the game.</param>
		void init(const EntityType* type, int entityID);

		/// <summary>
		/// Return entity type
		/// <summary>
		const EntityType& getEntityType() const { return *type; }

		/// <summary>
		/// Return entity type ID
		/// <summary>
		int getEntityTypeID() const;
		
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
		const std::vector<ActionInfo>& getAttachedActions() const { return attachedActions; }

		/// <summary>
		/// Retrieves the list of action types this entity can execute.
		/// </summary>
		/// <param name="gameInfo">Reference to the game information object (from GameState)</param>
		/// <returns>Vector with all action types this entity can execute</returns>
		std::vector<ActionType> getActionTypes(const GameInfo& gameInfo) const;

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
		// Prints information about the entity, parameters and actions
		/// <summary>
		void printInfo() const;

		/// <summary>
		/// Gets the list of parameters values. Modifiable.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		std::unordered_map<int, double>& getParamValues() { return parameters; }
		
		/// <summary>
		/// Gets the list of parameters values.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		const std::unordered_map<int, double>& getParamValues() const { return parameters; }

		/// <summary>
		/// Gets the list of parameters names.
		/// <summary>
		/// <returns>The list of parameters names of this entity.</returns>
		std::vector<std::string> getEntityParameterNames() const;

		/// <summary>
		/// Gets a map with all pairs <parameter,value>
		/// <summary>
		/// <returns>Returns a map with all the parameters.</returns>
		std::unordered_map<std::string, double> getEntityParameters() const;

		/// <summary>
		/// Gets the value of a specific parameter with buffs applied, by name 
		/// <summary>
		/// <returns>The parameter value.</returns>
	    double getParameter(const std::string& paramName) const;

		/// <summary>
		/// Gets the reference value of a specific parameter raw, by name 
		/// <summary>
		/// <returns>The parameter reference value.</returns>
		double& getRawParameter(const std::string& paramName);
		
		/// <summary>
		/// Gets a specific parameters raw, by index 
		/// <summary>
		/// <returns>The parameter reference value.</returns>
		double& getRawParameterAt(int paramIdx) { return parameters[paramIdx]; }

		/// <summary>
		/// Gets a specific parameters value, by index 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double& getParameterAt(int paramIdx) { return parameters[paramIdx]; }
		
		/// <summary>
		/// Gets a specific max parameters value, by index 
		/// <summary>
		/// <returns>The max parameter value.</returns>
		double getMaxParameterAt(int paramIdx) { return maxParameters[paramIdx]; }
		
		/// <summary>
		/// Gets a specific min parameters value, by index 
		/// <summary>
		/// <returns>The min parameter value.</returns>
		double getMinParameterAt(int paramIdx) { return minParameters[paramIdx]; }

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

		/// <summary>
		/// Gets the list of buffs attached to this entity. Modifiable.
		/// <summary>
		/// <returns>The list of buffs attached to this entity.</returns>
		std::vector<Buff>& getBuffs() { return buffs; }

		/// <summary>
		/// Gets the list of buffs attached to this entity. 
		/// <summary>
		/// <returns>The list of buffs attached to this entity.</returns>
		const std::vector<Buff>& getBuffs() const { return buffs; }

		/// <summary>
		/// Returns the movement speed of this entity.
		/// </summary>
		double getMovementSpeed() const { return movementSpeed; }

		/// <summary>
		/// Returns the collision radius of this entity.
		/// </summary>
		double getCollisionRadius() const { return collisionRadius; }

		/// <summary>
		/// Returns theID of this entity.
		/// </summary>
		int getID() const { return id; }

		/// <summary>
		/// Returns the owner ID (i.e. player ID) who controls this entity.
		/// </summary>
		int getOwnerID() const { return ownerID; }

		/// <summary>
		/// Sets the owner ID (player ID) in control of this entity..
		/// </summary>
		void setOwnerID(int oID) { ownerID = oID; }

		/// <summary>
		/// Returns the line of sight of this entity.
		/// </summary>
		double getLineOfSightRange() const { return lineOfSightRange; }


		/// <summary>
		/// Returns x position of this entity.
		/// </summary>
		double x() const { return position.x; }

		/// <summary>
		/// Returns y position of this entity.
		/// </summary>
		double y() const { return position.y; }

		/// <summary>
		/// Returns position of this entity.
		/// </summary>
		const Vector2f& getPosition() const { return position; }

		/// <summary>
		/// Sets the position of this entity in the board. Does not modify the board.
		/// </summary>
		void setPosition(Vector2f v) { position = v; }


		/// <summary>
		/// Returns the path that this entity is following (RTS games only) 
		/// </summary>
		const Path& getPath() const { return path; }

		/// <summary>
		/// Increments the current index of the path that this entity is following (RTS games only) 
		/// </summary>
		void incPathIndex() { path.currentPathIndex++; }

		/// <summary>
		/// Sets the path this entity is following (RTS games only) 
		/// </summary>
		void setPath(Path p) { path = p; }

		/// <summary>
		/// Add buff to the player
		/// <summary>
		void addBuff(Buff b)
		{
			buffs.emplace_back(b);
		}

		/// <summary>
		/// Recompute all the parameteres with the applied buffs
		/// <summary>
		void recomputeStats();

		/// <summary>
		/// Remove all the buffs of the same type
		/// <summary>
		void removeBuffsOfType(const BuffType& newType)
		{
			auto it = buffs.begin();
			while (it != buffs.end())
			{
				if (it->getType().getID() == newType.getID())
				{
					it = buffs.erase(it);
				}
				else it++;
			}
		}

		/// <summary>
		/// Recompute all the buffs
		/// <summary>
		void removeAllBuffs()
		{
			buffs.clear();
		}

		/// <summary>
		/// Check if player has a buff type applied
		/// <summary>
		bool hasBuff(int typeID) const
		{
			for (auto& buff : buffs)
			{
				if (buff.getType().getID() == typeID)
					return true;
			}

			return false;
		}
	};
}