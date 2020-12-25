#pragma once
#include <vector>
#include <ForwardModel/ActionTarget.h>

namespace SGA
{
	class TBSForwardModel;
	class RTSForwardModel;
	
	struct Action
	{
		Action():
			isEndAction(false),
			actionTypeID(-1),
			owner(0)
		{
		}

		//RTS Endtick && TBS Endturn
		bool isEndAction;
		
		int actionTypeID;
		// Contains all targets involved in an action
		// UnitAction: Index 0 contains the source and Index 1 the target of the action//opposite
		// PlayerAction": Index 0 contains the target of the action
		std::vector<ActionTarget> targets;
		int owner;
		//void execute(GameState& state) const;
		void execute(GameState& state, const TBSForwardModel& fm) const;
		void execute(GameState& state, const RTSForwardModel& fm) const;

		static Action createEndAction(int playerID)
		{
			Action a;
			a.owner = playerID;
			a.isEndAction = true;
			return a;
		}
	};
}