#pragma once
#include <ForwardModel/AbstractFM/ActionTarget.h>
#include <ForwardModel/AbstractFM/TargetType.h>
#include <Representation/Vector2.h>

#include <unordered_set>
#include <vector>

namespace SGA
{
	enum class ShapeType
	{
		Circle,
		Square,
		Cross,
		Line
	};

	struct TargetType
	{
		enum Type { None, Position, Entity };
		
		Type type;

		// Specific variables for targetType=Position
		ShapeType shapeType;
		int shapeSize;
		
		// Specific variables for targetType=Unit
		std::unordered_set<int> groupEntityTypes;

		operator Type() const { return type; }
	};
}
