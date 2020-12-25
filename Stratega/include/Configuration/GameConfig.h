#pragma once
#include <map>
#include <string>
#include <Configuration/ForwardModelConfig.h>
#include <Agent/Agent.h>
#include <yaml-cpp/yaml.h>
#include <Representation/TBSGameState.h>
#include <Configuration/FunctionParser.h>
#include <Configuration/IBoardGenerator.h>
namespace SGA
{
    struct GameConfig
    {
    	// Game information
        std::string gameType;
        int roundLimit = 100;
        int numPlayers = -1;
    	// Other stuff
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::unordered_map<int, TileType> tileTypes;
        std::unique_ptr<IBoardGenerator> boardGenerator;
        ForwardModelConfig forwardModelConfig;
    	// Entities
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::vector<int>> entityGroups;
        std::unordered_map<int, EntityType> entityTypes;
    	// Actions
        std::unordered_map<int, ActionType> actionTypes;

        std::vector<std::unique_ptr<Agent>> generateAgents() const;
    	
        int getNumberOfPlayers() const
        {
            return numPlayers == -1 ? agentParams.size() : numPlayers;
        }

        int getEntityID(const std::string& name) const;
        int getTileID(const std::string& name) const;
        int getActionID(const std::string& name) const;
    };
	
    std::unique_ptr<Game> generateAbstractGameFromConfig(const GameConfig& config, std::mt19937& rngEngine);

    std::unique_ptr<TBSGameState> generateAbstractTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
    std::unique_ptr<RTSGameState> generateAbstractRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
}