#pragma once
#include <string>
#include <yaml-cpp/node/node.h>
#include <Representation/TileType.h>

namespace SGA
{
	struct TileConfig
	{
        char symbol;
		bool isWalkable;

		TileConfig():
			symbol('\0'),
			isWalkable(true)
		{
		}
	};
	
    TileType tileTypeFromConfig(const TileConfig& config, int id);
}

namespace YAML
{
    template<>
    struct convert<SGA::TileConfig>
	{
        static bool decode(const Node& node, SGA::TileConfig& rhs)
    	{
            if (!node.IsMap())
                return false;

            // Required
        	
        	// Optional
            rhs.symbol = node["Symbol"].as<char>(rhs.symbol);
            rhs.isWalkable = node["IsWalkable"].as<bool>(rhs.isWalkable);
            return true;
        }
    };
}