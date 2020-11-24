#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ForwardModel/AbstractFM/TargetType.h>
#include <ForwardModel/AbstractFM/Effect.h>

namespace SGA
{
	class Precondition;
	
	struct ActionType
	{
		std::string name;
		int id;
		TargetType type;

		std::vector<int> groupEntityTypes;

		std::vector<std::unique_ptr<Precondition>> preconditions;
		std::vector<std::unique_ptr<Effect>> effects;
	};
}