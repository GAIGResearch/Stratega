#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <unordered_set>
#include <Stratega/Representation/Parameter.h>
#include <Stratega/Representation/Entity.h>

namespace SGA
{
	typedef int EntityTypeID;
	/// <summary>
	/// Contains the definition of the entity type. This information includes a list of parameters definitions,
	/// the actions that can execute, required technologies to spawn a entity, a list of spawnable entity types and
	/// a list of cost assigned to this entity type.
	/// </summary>
	struct EntityType
	{
		/// <summary>
		/// Unique entity type ID
		/// </summary>
		int id;

		/// <summary>
		/// Name of this type, as defined in the YAML file.
		/// </summary>
		std::string name;

		/// <summary>
		/// Symbol for this entity used in YAML for level placement.
		/// </summary>
		char symbol;

		/// <summary>
		/// Map of parameters of this entity. Key is an integer (ID of the parameter) and value is the paramter object.
		/// The parameter defines the min, max and default value this parameter can take.
		/// </summary>
		std::unordered_map<ParameterID, Parameter> parameters;

		/// <summary>
		/// List of action IDs this entity can execute.
		/// </summary>
		std::vector<int> actionIds;

		/// <summary>
		/// ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.
		/// </summary>
		int requiredTechnologyID;

		/// <summary>
		/// List of entity types this entity can spwan in the game.
		/// </summary>
		std::unordered_set<EntityTypeID> spawnableEntityTypes;

		/// <summary>
		/// List of parameters and values that must be spent to spawn/build/create this unit.
		/// </summary>
		std::unordered_map<ParameterID, double> cost;

		/// <summary>
		/// Continuous Action Time
		/// </summary>
		double continuousActionTime;
		
		/// <summary>
		/// Range of the line of sight of this unit.
		/// </summary>
		double lineOfSightRange;

		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameter(ParameterID id) const;

		/// <summary>
		/// Checks if this entity type is allowed to execute an action type
		/// </summary>
		bool canExecuteAction(int actionTypeID) const;

		/// <summary>
		/// Generate a new empty instance of this entity type 
		/// </summary>
		Entity instantiateEntity(int entityID) const;

		/// <summary>
		/// Returns the maximum value of a given parameter
		/// </summary>
		double getParamMax(std::string paramName) const;

		/// <summary>
		/// Returns the minimum value of a given parameter
		/// </summary>
		double getParamMin(std::string paramName) const;
	};
}
