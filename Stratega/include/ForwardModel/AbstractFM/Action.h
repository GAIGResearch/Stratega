#pragma once
#include <vector>
#include <ForwardModel/AbstractFM/ActionTarget.h>

namespace SGA
{
	struct Action
	{
		int actionTypeID;
		std::vector<ActionTarget> targets;

		void execute(GameState& state) const;
	};
}