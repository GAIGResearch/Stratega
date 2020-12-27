#pragma once
#include <ForwardModel/Effect.h>
#include <ForwardModel/Condition.h>
#include <ForwardModel/ActionSourceType.h>
#include <ForwardModel/TargetType.h>

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
		std::vector<std::shared_ptr<Condition>> targetConditions;
		std::vector<std::shared_ptr<Effect>> effects;
	};
}
