#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/Precondition.h>
#include <ForwardModel/AbstractFM/TargetType.h>


namespace SGA
{
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