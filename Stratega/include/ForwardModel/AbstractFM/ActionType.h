#pragma once
#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/Precondition.h>
#include <ForwardModel/AbstractFM/ActionSourceType.h>
#include <ForwardModel/AbstractFM/Target.h>

#include <string>
#include <vector>
#include <memory>

namespace SGA
{
	struct ActionType
	{
		std::string name;
		int id;
		ActionSourceType sourceType;

		Target actionTargets;
		std::vector<std::unique_ptr<Precondition>> preconditions;
		std::vector<std::unique_ptr<Effect>> effects;
	};
}
