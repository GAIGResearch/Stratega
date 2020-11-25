#pragma once
#include <ForwardModel/AbstractFM/ActionTarget.h>
#include <ForwardModel/AbstractFM/TargetType.h>
#include <ForwardModel/AbstractFM/ShapeType.h>

#include <unordered_set>
#include <vector>

namespace SGA
{
	struct Target
	{
		TargetType targetType;
		// Specific variables for targetType=Position
		ShapeType shapeType;
		int shapeSize;
		// Specific variables for targetType=Unit
		std::unordered_set<int> groupEntityTypes;
	};
}
