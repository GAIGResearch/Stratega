#pragma once
#include <map>
#include <string>
#include <Configuration/TileConfig.h>
#include <Configuration/BoardConfig.h>
#include <Configuration/UnitConfig.h>
#include <Configuration/ForwardModelConfig.h>
#include <ForwardModel/TBSForwardModel.h>
#include <Agent/Agent.h>

namespace SGA
{
	struct GameConfig
	{
        std::string gameType;
        std::vector<std::string> agentNames;
		std::map<std::string, TileConfig> tileTypes;
        std::map<std::string, UnitConfig> unitTypes;
        BoardConfig boardConfig;
        ForwardModelConfig forwardModelConfig;
        int roundLimit = 100;
        int numPlayers = 2;
	};

    std::vector<std::unique_ptr<Agent>> agentsFromConfig(const GameConfig& config);
    std::unordered_map<std::string, TileType> tileTypesFromConfig(const GameConfig& config);
    std::unordered_map<std::string, UnitType> unitTypesFromConfig(const GameConfig& config);
    TBSForwardModel forwardModelFromConfig(const GameConfig& config);
    std::unique_ptr<IBoardGenerator> boardGeneratorFromConfig(const GameConfig& config);
    std::unique_ptr<TBSGameState> generateTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
    std::unique_ptr<RTSGameState> generateRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
    std::unique_ptr<Game> generateGameFromConfig(const GameConfig& config, std::mt19937& rngEngine);
}

namespace YAML
{
    template<>
    struct convert<SGA::GameConfig>
    {
        static bool decode(const Node& node, SGA::GameConfig& rhs)
        {
            rhs.agentNames = node["Agents"].as<std::vector<std::string>>();
            rhs.tileTypes = node["Tiles"].as<std::map<std::string, SGA::TileConfig>>();
            rhs.unitTypes = node["Units"].as<std::map<std::string, SGA::UnitConfig>>();
            rhs.boardConfig = node["Board"].as<SGA::BoardConfig>();
            rhs.forwardModelConfig = node["ForwardModel"].as<SGA::ForwardModelConfig>();
            rhs.gameType = node["GameConfig"]["Type"].as<std::string>();
        	
        	if(node["GameConfig"].IsDefined())
        	{
                rhs.roundLimit = node["GameConfig"]["RoundLimit"].as<int>(100);
                rhs.numPlayers= node["GameConfig"]["PlayerCount"].as<int>(rhs.numPlayers);
        	}
        	
        	return true;
        }
    };
}
