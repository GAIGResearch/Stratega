#pragma once
#include <ForwardModel/TargetType.h>
#include <yaml-cpp/yaml.h>

namespace YAML
{
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
            else
                return false;

            return true;
        }
    };
}