#pragma once
#include <ForwardModel/AbstractFM/ActionType.h>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	struct ActionTypeConfig
	{
		ActionSourceType sourceType;
		std::vector<std::string> preconditions;
		std::vector<std::string> effects;
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::ActionTypeConfig>
    {
        static bool decode(const Node& node, SGA::ActionTypeConfig& rhs)
        {
            rhs.sourceType = node["Type"].as<SGA::ActionSourceType>();
            rhs.preconditions = node["Preconditions"].as<std::vector<std::string>>(std::vector<std::string>());
            rhs.effects = node["Effects"].as<std::vector<std::string>>(std::vector<std::string>());
            return true;
        }
    };

    template<>
    struct convert<SGA::ActionSourceType>
    {
        static bool decode(const Node& node, SGA::ActionSourceType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "EntityAction")
                rhs = SGA::ActionSourceType::Unit;
            else if (value == "PlayerAction")
                rhs = SGA::ActionSourceType::Player;
            else
                return false;
        	
            return true;
        }
    };
}
