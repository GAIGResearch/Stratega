#pragma once
#include <unordered_set>
#include <vector>

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
		
		Type type;

		// Specific variables for targetType=Position
		ShapeType shapeType;
		int shapeSize;
		
		// Specific variables for targetType=Unit
		std::unordered_set<int> groupEntityTypes;
		
		// Specific variables for targetType=Technology
		std::unordered_set<int> technologyTypes;

		operator Type() const { return type; }

		//Check if is a valid target
		bool isValid(const GameState& state, const ActionTarget& actionTarget, const ActionTarget& sourceActionTarget) const;

	};
}
