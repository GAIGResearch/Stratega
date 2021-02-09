#pragma once
#include <ForwardModel/ActionTarget.h>
#include <ForwardModel/TargetType.h>
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

		/*bool operator==(const TargetType& p) const
		{
			return type == p.type;
		}*/
	};

	//class MyHashFunction {
	//public:

	//	// Use sum of lengths of first and last names 
	//	// as hash function. 
	//	size_t operator()(const TargetType& p) const
	//	{
	//		return p.first.length() + p.last.length();
	//	}
	//};
}
