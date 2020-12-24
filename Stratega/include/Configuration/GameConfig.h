#pragma once
#include <map>
#include <string>
#include <Configuration/TileConfig.h>
#include <Configuration/BoardConfig.h>
#include <Configuration/ForwardModelConfig.h>
#include <Agent/Agent.h>
#include <yaml-cpp/yaml.h>
#include <Representation/TBSGameState.h>
#include <Configuration/EntityTypeConfig.h>
#include <Configuration/ActionTypeConfig.h>
#include <Configuration/FunctionParser.h>
#include <Configuration/TargetTypeConfig.h>
namespace SGA
{
    struct GameConfig
    {
        std::string gameType;
        std::vector<std::pair<std::string, YAML::Node>> agentParams;
        std::map<std::string, TileConfig> tileTypes;
        BoardConfig boardConfig;
        ForwardModelConfig forwardModelConfig;
        int roundLimit = 100;
        int numPlayers = -1;

        int getNumberOfPlayers() const
        {
            return numPlayers == -1 ? agentParams.size() : numPlayers;
        }

        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::vector<int>> entityGroups;

        std::unordered_map<int, EntityType> entityTypes;
        std::unordered_map<int, ActionType> actionTypes;

        int getEntityID(const std::string& name) const;
        int getActionID(const std::string& name) const;
    };
	
    std::vector<std::unique_ptr<Agent>> agentsFromConfig(const GameConfig& config);
    std::unordered_map<std::string, TileType> tileTypesFromConfig(const GameConfig& config);
    std::unique_ptr<IBoardGenerator> boardGeneratorFromConfig(const GameConfig& config);
    std::unique_ptr<Game> generateAbstractGameFromConfig(const GameConfig& config, std::mt19937& rngEngine);

	//Abstract

    std::unique_ptr<TBSGameState2> generateAbstractTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
    std::unique_ptr<RTSGameState2> generateAbstractRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine);
}

namespace YAML
{
    template<>
    struct convert<SGA::GameConfig>
    {
        static bool decode(const Node& node, SGA::GameConfig& rhs)
        {
        	for(auto agentNode : node["Agents"])
        	{
        		if(agentNode.IsScalar())
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
            rhs.boardConfig = node["Board"].as<SGA::BoardConfig>();
            rhs.forwardModelConfig = node["ForwardModel"].as<SGA::ForwardModelConfig>();
            rhs.gameType = node["GameConfig"]["Type"].as<std::string>();
        	
        	if(node["GameConfig"].IsDefined())
        	{
                rhs.roundLimit = node["GameConfig"]["RoundLimit"].as<int>(100);
                rhs.numPlayers= node["GameConfig"]["PlayerCount"].as<int>(rhs.numPlayers);
        	}

        	//New config
            if (!parseEntities(node, rhs) || !parseEntityGroups(node, rhs) || !parseActions(node, rhs))
                return false;
        	
            // Assign actions
            auto types = node["Entities"].as<std::map<std::string, SGA::EntityTypeConfig>>();
            for (auto& type : rhs.entityTypes)
            {
                for (const auto& actionName : types[type.second.name].actions)
                {
                    type.second.actionIds.emplace_back(rhs.getActionID(actionName));
                }
            }

            return true;
        }

        static bool parseEntities(const Node& node, SGA::GameConfig& rhs)
        {
            auto types = node["Entities"].as<std::map<std::string, SGA::EntityTypeConfig>>();
            for (const auto& nameTypePair : types)
            {
                SGA::EntityType type;
                type.name = nameTypePair.first;
                type.id = rhs.entityTypes.size();
                for (const auto& nameParamPair : nameTypePair.second.parameters)
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

        static bool parseEntityGroups(const Node& node, SGA::GameConfig& rhs)
        {
            if (!node["EntityGroups"])
                return true;

            auto groups = node["EntityGroups"].as<std::map<std::string, std::vector<std::string>>>();
            for (const auto& nameGroupPair : groups)
            {
                rhs.entityGroups.emplace(nameGroupPair.first, std::vector<int>());
                for (const auto& entityName : nameGroupPair.second)
                {
                    rhs.entityGroups[nameGroupPair.first].emplace_back(rhs.getEntityID(entityName));
                }
            }

            // Group that contains all entities
            rhs.entityGroups.emplace("All", std::vector<int>());
            for (const auto& idEntityPair : rhs.entityTypes)
            {
                rhs.entityGroups.at("All").emplace_back(idEntityPair.first);
            }

            return true;
        }

        static bool parseActions(const Node& node, SGA::GameConfig& rhs)
        {
            SGA::FunctionParser parser;
            auto types = node["Actions"].as<std::map<std::string, SGA::ActionTypeConfig>>();

            //Add actionType
            SGA::ActionType actionType;
            actionType.id = 0;
            actionType.name = "EndTurn";
            actionType.sourceType = SGA::ActionSourceType::Player;

           rhs.actionTypes.emplace(0, std::move(actionType));
        	
            for (const auto& nameTypePair : types)
            {
                SGA::ActionType type;
                type.id = rhs.actionTypes.size();
                type.name = nameTypePair.first;
                type.actionTargets = parseTargetType(nameTypePair.second.targetNode, rhs);
                type.sourceType = nameTypePair.second.sourceType;
                std::unordered_map<std::string, int> actionTargetIds;
                actionTargetIds.emplace("Source", 0);
                actionTargetIds.emplace("Target", 1);

                // Parse preconditions
                parser.parseFunctions(nameTypePair.second.preconditions, type.preconditions, actionTargetIds, rhs.parameters);

                // Parse effects                
                parser.parseFunctions(nameTypePair.second.effects, type.effects, actionTargetIds, rhs.parameters);

                rhs.actionTypes.emplace(rhs.actionTypes.size(), std::move(type));
            }

            return true;
        }

        static SGA::TargetType parseTargetType(const Node& node, const SGA::GameConfig& rhs)
        {
            SGA::TargetType targetType;
            targetType.type = node["Type"].as<SGA::TargetType::Type>();
            if (targetType.type == SGA::TargetType::Position)
            {
                targetType.shapeType = node["Shape"].as<SGA::ShapeType>();
                targetType.shapeSize = node["Size"].as<int>();
            }
            else if (targetType.type == SGA::TargetType::Entity)
            {
                auto groupName = node["ValidTargets"].as<std::string>("All");
                const auto& group = rhs.entityGroups.at(groupName);
                targetType.groupEntityTypes.insert(group.begin(), group.end());
            }

            return targetType;
        }
    };
}
