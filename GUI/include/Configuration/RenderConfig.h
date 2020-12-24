#pragma once
#include <map>
#include <string>
#include <yaml-cpp/yaml.h>

#include "AbstractTBSGameStateRender.h"
#include "AbstractRTSGameStateRender.h"

namespace SGA
{
	struct GameConfig;

	struct RenderConfig
	{
		std::map<std::string, std::string> entitySpritePaths;
		std::map<std::string, std::string> tileSpritePaths;
	};

    std::unique_ptr<GameStateRenderBase> stateRendererFromConfig(Game& game, const RenderConfig& config, const GameConfig& gameConfig, int playerID);
}

namespace YAML
{
    template<>
    struct convert<SGA::RenderConfig>
    {
        static bool decode(const Node& node, SGA::RenderConfig& rhs)
        {
            if (!node.IsMap())
                return false;
            
        	for(const auto& entityNode : node["Entities"])
        	{
                auto entityName = entityNode.first.as<std::string>();
                auto entityConfig = entityNode.second;
                rhs.entitySpritePaths.emplace(entityName, entityConfig["Sprite"].as<std::string>());
        	}

            for (const auto& tileNode : node["Tiles"])
            {
                auto tileName = tileNode.first.as<std::string>();
                auto tileConfig = tileNode.second;
                rhs.tileSpritePaths.emplace(tileName, tileConfig["Sprite"].as<std::string>());
            }

            return true;
        }
    };
}
