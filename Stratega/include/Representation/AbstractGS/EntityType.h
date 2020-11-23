#pragma once
#include <string>
#include <unordered_map>
#include <Representation/AbstractGS/Parameter.h>

namespace SGA
{
	struct EntityType
	{
		int id;
		std::string name;
		// ToDo This forces us to do string-lookups which can be quite slow when done often, replace with integer
		std::unordered_map<std::string, Parameter> parameters;
	};
}
