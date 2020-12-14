#pragma once
#include <map>
#include <string>
#include <Configuration/TileConfig.h>
#include <Configuration/BoardConfig.h>
#include <Configuration/UnitConfig.h>
#include <Configuration/ForwardModelConfig.h>
#include <ForwardModel/TBSForwardModel.h>
#include <Agent/Agent.h>
#include <yaml-cpp/yaml.h>
#include <Configuration/AbstractConfig/ActionTypeConfig.h>
#include <Configuration/AbstractConfig/EntityTypeConfig.h>

#include "FunctionParser.h"

namespace SGA
{
    struct GameConfig2
    {
        std::string gameType;
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::map<std::string, TileConfig> tileTypes;
        std::map<std::string, UnitConfig> unitTypes;
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<int, EntityType> entityTypes;
        std::unordered_map<int, ActionType> actionTypes;
        BoardConfig boardConfig;
        ForwardModelConfig forwardModelConfig;
        int roundLimit = 100;
        int numPlayers = -1;

        std::vector<std::unique_ptr<Agent>> generateAgents();
        std::unordered_map<std::string, TileType> generateTileTypes();
        std::unordered_map<std::string, UnitType> generateUnitTypes();
        TBSForwardModel generateTBSforwardModel();
        RTSForwardModel generateRTSforwardModel();
        std::unique_ptr<IBoardGenerator> generateBoardGenerator();
        std::unique_ptr<TBSGameState> generateTBSState(std::mt19937& rngEngine);
        std::unique_ptr<RTSGameState> generateRTSState(std::mt19937& rngEngine);
        std::unique_ptr<Game> generateGame(std::mt19937& rngEngine);

        int getNumberOfPlayers() const
        {
            return numPlayers == -1 ? agentParams.size() : numPlayers;
        }
    };
}

namespace YAML
{
    template<>
    struct convert<SGA::GameConfig2>
    {
        static bool decode(const Node& node, SGA::GameConfig2& rhs)
        {
            if (!parseEntities(node, rhs) || !parseActions(node, rhs))
                return false;
        	
            for (auto agentNode : node["Agents"])
            {
                if (agentNode.IsScalar())
                {
                    rhs.agentParams.emplace_back(agentNode.as<std::string>(), YAML::Null);
                }
                else
                {
                    auto map = agentNode.as<std::map<std::string, Node>>();
                    const auto& firstEntry = *map.begin();
                    rhs.agentParams.emplace_back(firstEntry);
                }
            }

            rhs.tileTypes = node["Tiles"].as<std::map<std::string, SGA::TileConfig>>();
            rhs.unitTypes = node["Units"].as<std::map<std::string, SGA::UnitConfig>>();
            rhs.boardConfig = node["Board"].as<SGA::BoardConfig>();
            rhs.forwardModelConfig = node["ForwardModel"].as<SGA::ForwardModelConfig>();
            rhs.gameType = node["GameConfig"]["Type"].as<std::string>();

            if (node["GameConfig"].IsDefined())
            {
                rhs.roundLimit = node["GameConfig"]["RoundLimit"].as<int>(100);
                rhs.numPlayers = node["GameConfig"]["PlayerCount"].as<int>(rhs.numPlayers);
            }

            return true;
        }

    	static bool parseEntities(const Node& node, SGA::GameConfig2& rhs)
        {
            auto types = node["Entities"].as<std::map<std::string, SGA::EntityTypeConfig>>();
        	for(const auto& nameTypePair : types)
        	{
                SGA::EntityType type;
                type.name = nameTypePair.first;
                type.id = rhs.entityTypes.size();
        		for(const auto& nameParamPair : nameTypePair.second.parameters)
        		{
                    // Assign IDs to parameters that do not exist yet
                    if (rhs.parameters.find(nameParamPair.first) == rhs.parameters.end())
                    {
                        rhs.parameters.insert({ nameParamPair.first, rhs.parameters.size() });
                    }

        			// Construct the parameter
                    SGA::Parameter param;
                    param.name = nameParamPair.first;
                    param.minValue = 0;
                    param.maxValue = nameParamPair.second;
                    param.defaultValue = param.maxValue;
                    param.index = type.parameters.size();
                    type.parameters.insert({ rhs.parameters.at(nameParamPair.first), std::move(param) });
        		}

                rhs.entityTypes.emplace(type.id, std::move(type));
        	}
        	
            return true;
        }

    	static bool parseActions(const Node& node, SGA::GameConfig2& rhs)
        {
            SGA::FunctionParser parser;
            auto types = node["Actions"].as<std::map<std::string, SGA::ActionTypeConfig>>();
            for(const auto& nameTypePair : types)
            {
                SGA::ActionType type;

            	// Parse preconditions
                std::unordered_map<std::string, int> actionTargetIds;
                actionTargetIds.emplace("Source", 0);
            	for(const auto& precondition : nameTypePair.second.preconditions)
            	{
                    auto functionCall = parser.parseFunction(precondition, actionTargetIds, rhs.parameters);

            		// ToDo Construct precondition
            	}

            	// Parse effects
                actionTargetIds.emplace("Target", 1);
                for (const auto& effect : nameTypePair.second.effects)
                {
                    auto functionCall = parser.parseFunction(effect, actionTargetIds, rhs.parameters);

                    // ToDo Construct effect
                }
            }
        	
            return true;
        }
    };
}
