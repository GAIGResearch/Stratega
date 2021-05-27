#pragma once
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Representation/LevelDefinition.h>

namespace SGA
{
	
    std::unique_ptr<GameConfig> loadConfigFromYAML(const std::string& filePath);
    std::unordered_map<int, LevelDefinition> loadLevelsFromYAML(const std::string& fileMapsPath, const GameConfig& config);
	
	class GameConfigParser
	{
	public:
		std::unique_ptr<GameConfig> parseFromFile(const std::string& filePath) const;
        std::unordered_map<int, LevelDefinition> parseLevelsFromFile(const std::string& fileMapsPath, const GameConfig& config) const;

		void parseAgents(const YAML::Node& agentsNode, GameConfig& config) const;
		void parseTileTypes(const YAML::Node& tilesNode, GameConfig& config) const;
		//If the method received a maps path, it will load them from the path instead of the game yaml
		void parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const;
		void parseEntities(const YAML::Node& entitiesNode, GameConfig& config) const;
        void parseEntityGroups(const YAML::Node& entityGroupsNode, GameConfig& config) const;
        void parseActions(const YAML::Node& actionsNode, GameConfig& config) const;
        void parseForwardModel(const YAML::Node& fmNode, GameConfig& config) const;
        void parsePlayers(const YAML::Node& parametersNode, GameConfig& config) const;
        void parseTechnologyTrees(const YAML::Node& techtreeNode, GameConfig& config) const;
		void parseGameDescription(const YAML::Node& gameDescription, GameConfig& config) const;
        void parseRenderConfig(const YAML::Node& configNode, GameConfig& config) const;
		
	private:
        std::unordered_set<EntityTypeID> parseEntityGroup(const YAML::Node& groupNode, const GameConfig& config) const;
        std::unordered_map<ParameterID, double> parseCost(const YAML::Node& costNode, const GameConfig& config) const;
		TargetType parseTargetType(const YAML::Node& node, const GameConfig& config) const;
		ActionCategory parseActionCategory(const std::string& name) const;
        EntityCategory parseEntityCategory(const std::string& name) const;
        void parseParameterList(const YAML::Node& parameterNode, GameConfig& config, std::unordered_map<ParameterID, Parameter>& parameterBucket) const;
        std::string parseFilePath(const YAML::Node& pathNode, const GameConfig& config) const;
        void parseMaps(const YAML::Node& mapsLayout, std::unordered_map<int, LevelDefinition>& levelDefinitions, const GameConfig& config) const;
        void parseLevelDefinition(const YAML::Node& mapLayout, std::string mapName, std::unordered_map<int, LevelDefinition>& levelDefinitions, const GameConfig& config) const;
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::GameType>
    {
        static bool decode(const Node& node, SGA::GameType& rhs)
        {
            if (!node.IsScalar())
                return false;
        	
            auto value = node.as<std::string>();
            if (value == "RTS")
                rhs = SGA::GameType::RTS;
            else if (value == "TBS")
                rhs = SGA::GameType::TBS;
            else
                return false;

            return true;
        }
    };
	
    template<>
    struct convert<SGA::ActionSourceType>
    {
        static bool decode(const Node& node, SGA::ActionSourceType& rhs)
        {
            if (!node.IsScalar())
                return false;
            auto value = node.as<std::string>();
            if (value == "EntityAction")
                rhs = SGA::ActionSourceType::Entity;
            else if (value == "PlayerAction")
                rhs = SGA::ActionSourceType::Player;
            else
                return false;

            return true;
        }
    };

    template<>
    struct convert<SGA::TargetType::Type>
    {
        static bool decode(const Node& node, SGA::TargetType::Type& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "Position")
                rhs = SGA::TargetType::Type::Position;
            else if (value == "Entity")
                rhs = SGA::TargetType::Type::Entity;
            else if (value == "EntityType")
                rhs = SGA::TargetType::Type::EntityType;
            else if (value == "Technology")
                rhs = SGA::TargetType::Type::Technology;
            else if (value == "ContinuousAction")
                rhs = SGA::TargetType::Type::ContinuousAction;
            else
                return false;

            return true;
        }
    };
	
    template<>
    struct convert<SGA::ShapeType>
    {
        static bool decode(const Node& node, SGA::ShapeType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "Circle")
                rhs = SGA::ShapeType::Circle;
            else if (value == "Square")
                rhs = SGA::ShapeType::Square;
            else if (value == "Cross")
                rhs = SGA::ShapeType::Cross;
            else if (value == "Line")
                rhs = SGA::ShapeType::Line;
            else if (value == "AllPositions")
                rhs = SGA::ShapeType::AllPositions;
            else
                return false;

            return true;
        }
    };

    template<>
    struct convert<SGA::Neighbours::ShapeType>
    {
        static bool decode(const Node& node, SGA::Neighbours::ShapeType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "Circle")
                rhs = SGA::Neighbours::ShapeType::Circle;
            else if (value == "Square")
                rhs = SGA::Neighbours::ShapeType::Square;
            else if (value == "AllPositions")
                rhs = SGA::Neighbours::ShapeType::AllPositions;
            else
                return false;

            return true;
        }
    };
	
    template<>
    struct convert<std::shared_ptr<SGA::SamplingMethod>>
    {
        static bool decode(const Node& node, std::shared_ptr<SGA::SamplingMethod>& rhs)
        {

            auto value = node["Type"].as<std::string>();
            if (value == "Neighbours")
            {
                auto& temp = std::make_shared<SGA::Neighbours>();
                if (node["Options"].IsDefined())
                {
                    temp->shapeType = node["Options"]["Shape"].as<SGA::Neighbours::ShapeType>();
                    temp->shapeSize = node["Options"]["Size"].as<int>(0);
                }

                rhs = temp;
            }                
            else if (value == "Dijkstra")
            {
                auto& temp = std::make_shared<SGA::Dijkstra>();
                if (node["Options"].IsDefined())
                {
                    temp->searchSize = node["Options"]["SearchSize"].as<int>(1);
                    temp->allowDiagonals = node["Options"]["AllowDiagonals"].as<bool>(false);
                }

                rhs = temp;
            }               
            else
                return false;

            return true;
        }
    };
}