#pragma once
//#include <yaml-cpp/node/node.h>
#include <Configuration/YamlHeaders.h>
namespace SGA
{
	enum class WinConditionType
	{
		LastManStanding,
		UnitAlive
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::WinConditionType>
    {
        static bool decode(const Node& node, SGA::WinConditionType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "LastManStanding")
            {
                rhs = SGA::WinConditionType::LastManStanding;
            }
            else if(type=="UnitAlive")
            {
                rhs = SGA::WinConditionType::UnitAlive;
            }
            else
            {
                return false;
            }

            return true;
        }
    };
}