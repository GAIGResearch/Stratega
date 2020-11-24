#pragma once
#include <ForwardModel/AbstractFM/TargetType.h>
#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/Precondition.h>

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace SGA
{
	struct ActionType
	{
		std::string name;
		int id;
		TargetType targetType;

		std::unordered_set<int> groupEntityTypes;

		std::vector<std::unique_ptr<Precondition>> preconditions;
		std::vector<std::unique_ptr<Effect>> effects;
	};
}