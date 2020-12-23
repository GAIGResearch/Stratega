#pragma once
#include <map>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	struct EntityTypeConfig
	{
		std::map<std::string, double> parameters;
		std::vector<std::string> actions;
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::EntityTypeConfig>
    {
        static bool decode(const Node& node, SGA::EntityTypeConfig& rhs)
        {
            rhs.parameters = node["Parameters"].as<std::map<std::string, double>>();
            rhs.actions = node["Actions"].as<std::vector<std::string>>();
            return true;
        }
    };
}

