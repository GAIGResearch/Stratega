#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ForwardModel/AbstractFM/TargetType.h>


namespace SGA
{
	class Precondition;
	class Effect;
	
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