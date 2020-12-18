#pragma once
#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/Condition.h>
#include <ForwardModel/AbstractFM/ActionSourceType.h>
#include <ForwardModel/AbstractFM/TargetType.h>

#include <string>
#include <vector>
#include <memory>

namespace SGA
{
	class Condition;
	
	struct ActionType
	{
		std::string name;
		int id;
		ActionSourceType sourceType;

		TargetType actionTargets;
		std::vector<std::shared_ptr<Condition>> preconditions;
		std::vector<std::shared_ptr<Condition>> condition;
		std::vector<std::shared_ptr<Effect>> effects;
	};
}
