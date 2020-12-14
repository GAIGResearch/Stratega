#pragma once
#include <map>
#include <string>
#include <unordered_map>

#include <Configuration/AbstractConfig/EntityTypeConfig.h>
#include <Configuration/AbstractConfig/ActionTypeConfig.h>
#include <Configuration/AbstractConfig/FunctionParser.h>
#include <Configuration/AbstractConfig/TargetTypeConfig.h>

namespace SGA
{
    struct GameConfig2
    {
        std::unordered_map<std::string, ParameterID> parameters;
        std::unordered_map<std::string, std::vector<int>> entityGroups;
    	
        std::unordered_map<int, EntityType> entityTypes;
        std::unordered_map<int, ActionType> actionTypes;

        int getEntityID(const std::string& name);
    };
}

namespace YAML
{
    template<>
    struct convert<SGA::GameConfig2>
    {
        static bool decode(const Node& node, SGA::GameConfig2& rhs)
        {
            if (!parseEntities(node, rhs) || !parseEntityGroups(node, rhs) || !parseActions(node, rhs))
                return false;

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

    	static bool parseEntityGroups(const Node& node, SGA::GameConfig2& rhs)
        {
            if (!node["EntityGroups"])
                return true;

            auto groups = node["EntityGroups"].as<std::map<std::string, std::vector<std::string>>>();
        	for(const auto& nameGroupPair : groups)
        	{
                rhs.entityGroups.emplace(nameGroupPair.first, std::vector<int>());
        		for(const auto& entityName : nameGroupPair.second)
        		{
                    rhs.entityGroups[nameGroupPair.first].emplace_back(rhs.getEntityID(entityName));
        		}
        	}

        	// Group that contains all entities
            rhs.entityGroups.emplace("All", std::vector<int>());
        	for(const auto& idEntityPair: rhs.entityTypes)
        	{
                rhs.entityGroups.at("All").emplace_back(idEntityPair.first);
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
            	type.id = rhs.actionTypes.size();
                type.name = nameTypePair.first;
                type.actionTargets = parseTargetType(nameTypePair.second.targetNode, rhs);

            	// Parse preconditions
                std::unordered_map<std::string, int> actionTargetIds;
                actionTargetIds.emplace("Source", 0);
                parser.parseFunctions(nameTypePair.second.preconditions, type.preconditions, actionTargetIds, rhs.parameters);

            	// Parse effects
                actionTargetIds.emplace("Target", 1);
                parser.parseFunctions(nameTypePair.second.effects, type.effects, actionTargetIds, rhs.parameters);

                rhs.actionTypes.emplace(rhs.actionTypes.size(), std::move(type));
            }
        	
            return true;
        }

    	static SGA::TargetType parseTargetType(const Node& node, const SGA::GameConfig2& rhs)
        {
            SGA::TargetType targetType;
            targetType.type = node["Type"].as<SGA::TargetType::Type>();
            if (targetType.type == SGA::TargetType::Position)
            {
                targetType.shapeType = node["Shape"].as<SGA::ShapeType>();
                targetType.shapeSize = node["Size"].as<int>();
            }
            else if(targetType.type == SGA::TargetType::Entity)
            {
                auto groupName = node["ValidTargets"].as<std::string>("All");
                const auto& group = rhs.entityGroups.at(groupName);
                targetType.groupEntityTypes.insert(group.begin(), group.end());
            }
        	
            return targetType;
        }
    };
}
