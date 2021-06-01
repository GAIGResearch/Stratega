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
		int id;
		std::string name;
		char symbol;
		std::unordered_map<ParameterID, Parameter> parameters;
		std::vector<int> actionIds;

		int requiredTechnologyID;
		std::unordered_set<EntityTypeID> spawnableEntityTypes;
		std::unordered_map<ParameterID, double> cost;
		double continuousActionTime;
		
		double lineOfSight;

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
