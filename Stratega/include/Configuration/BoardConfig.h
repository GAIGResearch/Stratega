#pragma once
#include <unordered_map>
#include <Configuration/BoardGenerationType.h>
#include <Configuration/IBoardGenerator.h>

namespace SGA
{
    struct BoardConfig
    {
        BoardGenerationType generationType;
        std::string layout;

        BoardConfig() :
            generationType(BoardGenerationType::None),
            layout("")
        {
        }
    };
}

namespace YAML
{
    template<>
    struct convert<SGA::BoardConfig>
    {
        static bool decode(const Node& node, SGA::BoardConfig& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.generationType = node["GenerationType"].as<SGA::BoardGenerationType>();
        	if(rhs.generationType == SGA::BoardGenerationType::Manual)
        	{
                rhs.layout = node["Layout"].as<std::string>();
        	}
        	
            return true;
        }
    };
}
