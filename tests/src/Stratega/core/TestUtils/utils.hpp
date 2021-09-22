#pragma once

#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
    inline YAML::Node loadAndGetNode(std::string filename, std::string nodeName)
    {
        auto node = YAML::LoadFile(filename);
        return node[nodeName];
    }

    inline YAML::Node loadFromStringAndGetNode(std::string yamlString, std::string nodeName)
    {
        auto node = YAML::Load(yamlString.c_str());
        return node[nodeName];
    }

    inline ActionType* findActionType(std::unordered_map<int, ActionType>& actionTypes, std::string name)
    {
        for (auto& type : actionTypes)
        {
            if (type.second.getName() == name)
            {
                return &type.second;
            }
        }

        return nullptr;
    }
    
    inline Parameter* findParameterType(std::unordered_map<ParameterID, Parameter>& playerParameters, std::string name)
    {
        for (auto& type : playerParameters)
        {
            if (type.second.getName() == name)
            {
                return &type.second;
            }
        }

        return nullptr;
    }

    inline EntityType* findEntityType(std::unordered_map<int, EntityType>& entityTypes, std::string name)
    {
        for (auto& type : entityTypes)
        {
            if (type.second.getName() == name)
            {
                return &type.second;
            }
        }

        return nullptr;
    }
}