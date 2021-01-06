#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <Representation/Parameter.h>
#include <Representation/Entity.h>

namespace SGA
{
	typedef int ParameterID;
	
	struct EntityType
	{
		int id;
		std::string name;
		std::unordered_map<ParameterID, Parameter> parameters;
		std::vector<int> actionIds;
		float lineOfSight;
		const Parameter& getParameter(ParameterID id) const;
		bool canExecuteAction(int actionTypeID) const;

		Entity instantiateEntity(int entityID) const;

	};
}
