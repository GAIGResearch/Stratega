#pragma once
#include <map>
#include <string>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Configuration/FunctionParser.h>
#include <Stratega/Configuration/RenderConfig.h>
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/GameDescription.h>
#include <yaml-cpp/yaml.h>

#include "yaml-cpp/node/node.h"

namespace SGA
{
    class LevelDefinition;
	
    struct GameConfig
    {
        // Rendering - ToDo Split renderConfig and move into dedicated parts (Tile, Entity, etc)
        std::unique_ptr<RenderConfig> renderConfig;
    	
    	// Game information
        GameType gameType = GameType::TBS;

        int tickLimit = 100;
        int numPlayers = -1;
    	
    	// Other stuff
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::unordered_map<int, TileType> tileTypes;

    	//ForwardModel
        std::unique_ptr<EntityForwardModel> forwardModel;
    	
    	// Players
        std::unordered_map<ParameterID, Parameter> playerParameterTypes;
        std::unordered_set<EntityTypeID> playerSpawnableTypes;
        std::vector<int> playerActionIds;
    	// Entities
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::unordered_set<int>> entityGroups;
        std::unordered_map<int, EntityType> entityTypes;
    	// Actions
        std::unordered_map<int, ActionType> actionTypes;
    	// State-Generation
        //std::string boardString;
        int defaultTileTypeID=-1;
        std::unordered_map<int, LevelDefinition> levelDefinitions;
        int selectedLevel;

    	// Technology tree
        TechnologyTreeCollection technologyTreeCollection;
            	
        std::vector<std::unique_ptr<Agent>> generateAgents() const;
        std::unique_ptr<GameState> generateGameState(int levelID=-1) const;

    	//ActionCategories
        std::unordered_map<ActionCategory, std::vector<int>> actionCategories;

        //EntityCategories
        std::unordered_map<EntityCategory, std::vector<int>> entityCategories;

    	//Yaml path
        std::string yamlPath;
    	
    	//Base utilities
        size_t getNumberOfPlayers() const;
        int getEntityID(const std::string& name) const;
        int getTileID(const std::string& name) const;
        int getActionID(const std::string& name) const;
        int getTechnologyID(const std::string& name) const;
    };
}
