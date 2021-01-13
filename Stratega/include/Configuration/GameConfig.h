#pragma once
#include <map>
#include <string>
#include <Agent/Agent.h>
#include <Representation/TBSGameState.h>
#include <Configuration/FunctionParser.h>
#include <ForwardModel/ForwardModel.h>
#include <Representation/TechnologyTree.h>
namespace SGA
{
    struct GameConfig
    {
    	// Game information
        ForwardModelType gameType;
        int tickLimit = 100;
        int numPlayers = -1;
    	// Other stuff
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::unordered_map<int, TileType> tileTypes;
        std::unique_ptr<EntityForwardModel> forwardModel;
    	// Entities
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::vector<int>> entityGroups;
        std::unordered_map<int, EntityType> entityTypes;
    	// Actions
        std::unordered_map<int, ActionType> actionTypes;
    	// State-Generation
        std::string boardString;
        int defaultTileTypeID;

    	//Technology tree
        TechnologyTreeCollection technologyTreeCollection;
    	
        std::vector<std::unique_ptr<Agent>> generateAgents() const;
        std::unique_ptr<GameState> generateGameState() const;
    	
        int getNumberOfPlayers() const;
        int getEntityID(const std::string& name) const;
        int getTileID(const std::string& name) const;
        int getActionID(const std::string& name) const;
        int getTechnologyID(const std::string& name) const;
    };
	
    std::unique_ptr<Game> generateAbstractGameFromConfig(const GameConfig& config, std::mt19937& rngEngine);
}