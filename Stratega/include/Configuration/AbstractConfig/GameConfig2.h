#pragma once
#include <yaml-cpp/node/node.h>
#include <regex>

namespace SGA
{
	struct GameConfig2
	{
		
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::GameConfig2>
    {
        static bool decode(const Node& node, SGA::GameConfig2& rhs)
        {
            auto actionConfigs = node["Actions"].as<std::map<std::string, YAML::Node>>();
            auto n1 = actionConfigs["Attack"];
            auto preconditions = n1["Preconditions"];

            bool isSequence = preconditions.IsSequence();
            bool isMap = preconditions.IsMap();
            bool isDefined = preconditions.IsDefined();
            bool isScalar = preconditions.IsScalar();
            bool isNull = preconditions.IsNull();

            auto test = preconditions.Scalar();
            auto test2 = preconditions.as<std::vector<std::string>>();
        	
            return true;
        }
    };
}