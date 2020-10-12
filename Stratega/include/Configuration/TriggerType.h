#pragma once
#include <yaml-cpp/node/node.h>

namespace SGA
{
	enum class TriggerType
	{
		EndOfTurn,
		EnterTile
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::TriggerType>
    {
        static bool decode(const Node& node, SGA::TriggerType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "EndOfTurn")
            {
                rhs = SGA::TriggerType::EndOfTurn;
            }
            else if(type == "EnterTile")
            {
                rhs = SGA::TriggerType::EnterTile;
            }
            else
            {
                return false;
            }

            return true;
        }
    };
}