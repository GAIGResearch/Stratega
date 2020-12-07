#pragma once
#include <vector>
#include <ForwardModel/AbstractFM/ActionTarget.h>

namespace SGA
{
	class TBSAbstractForwardModel;
	
	struct Action
	{
		int actionTypeID;
		// Contains all targets involved in an action
		// UnitAction: Index 0 contains the source and Index 1 the target of the action
		// PlayerAction": Index 0 contains the target of the action
		std::vector<ActionTarget> targets;
		int owner;
		void execute(GameState& state) const;
		void execute(GameState& state, const TBSAbstractForwardModel& fm) const;
	};
}