#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <Stratega/ForwardModel/SamplingMethod.h>
namespace SGA
{
	class ActionTarget;
	struct GameState;
	
	enum class ShapeType
	{
		Circle,
		Square,
		Cross,
		Line,
		AllPositions
	};

	struct TargetType
	{
		enum Type { None, Position, EntityType, Entity, Technology, ContinuousAction};
		
		Type type = Type::None;

		std::shared_ptr<SamplingMethod> samplingMethod;
		
		// Specific variables for targetType=Position
		ShapeType shapeType;
		int shapeSize;
		
		// Specific variables for targetType=Unit
		std::unordered_set<int> groupEntityTypes;
		
		// Specific variables for targetType=Technology
		std::unordered_set<int> technologyTypes;

		operator Type() const { return type; }

		std::string getTypeString() const
		{
			switch (type)
			{
			case None:
				return "None";
				break;
			case Position:
				return "Position";
			break;
			case EntityType:
				return "EntityType";
				break;
			case Entity:
				return "Entity";
				break;
			case Technology:
				return "Technology";
				break;
			case ContinuousAction:
				return "ContinuousAction";
				break;
			}
		}
		//Check if is a valid target
		bool isValid(const GameState& state, const ActionTarget& actionTarget, const ActionTarget& sourceActionTarget) const;

	};
}
