#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <unordered_set>
#include <Representation/Parameter.h>
#include <Representation/Entity.h>

namespace SGA
{
	typedef int EntityTypeID;
	
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
		
		float lineOfSight;
		
		const Parameter& getParameter(ParameterID id) const;
		bool canExecuteAction(int actionTypeID) const;
		Entity instantiateEntity(int entityID) const;

	};
}
