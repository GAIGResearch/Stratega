#pragma once
#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
    std::unique_ptr<GameConfig> loadConfigFromYAML(const std::string& filePath);
	
	class GameConfigParser
	{
	public:
		std::unique_ptr<GameConfig> parseFromFile(const std::string& filePath) const;

		void parseAgents(const YAML::Node& agentsNode, GameConfig& config) const;
		void parseTileTypes(const YAML::Node& tilesNode, GameConfig& config) const;
		void parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const;
		void parseEntities(const YAML::Node& entitiesNode, GameConfig& config) const;
        void parseEntityGroups(const YAML::Node& entityGroupsNode, GameConfig& config) const;
        void parseActions(const YAML::Node& actionsNode, GameConfig& config) const;
        void parseForwardModel(const YAML::Node& fmNode, GameConfig& config) const;
        void parsePlayers(const YAML::Node& parametersNode, GameConfig& config) const;
        void parseTechnologyTrees(const YAML::Node& techtreeNode, GameConfig& config) const;
		void parseActionCategories(const YAML::Node& gameDescription, GameConfig& config) const;

		
	private:
        std::unordered_set<EntityTypeID> parseEntityGroup(const YAML::Node& groupNode, const GameConfig& config) const;
        std::unordered_map<ParameterID, double> parseCost(const YAML::Node& costNode, const GameConfig& config) const;
		TargetType parseTargetType(const YAML::Node& node, const GameConfig& config) const;
		ActionCategory parseActionCategory(const std::string& name) const;
        void parseParameterList(const YAML::Node& parameterNode, GameConfig& config, std::unordered_map<ParameterID, Parameter>& parameterBucket) const;
		
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
}