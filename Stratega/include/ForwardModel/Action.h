#pragma once
#include <vector>
#include <ForwardModel/ActionTarget.h>

namespace SGA
{
	class EntityForwardModel;
	
	struct Action
	{
		Action():
			isEndAction(false),
			actionTypeID(-1),
			ownerID(0)
		{
		}

		//RTS Endtick && TBS Endturn
		bool isEndAction;
		
		int actionTypeID;
		// Contains all targets involved in an action
		// UnitAction: Index 0 contains the source and Index 1 the target of the action//opposite
		// PlayerAction": Index 0 contains the target of the action
		std::vector<ActionTarget> targets;
		int ownerID;
		
		void execute(GameState& state, const EntityForwardModel& fm) const;

		static Action createEndAction(int playerID)
		{
			Action a;
			a.actionTypeID = 0;
			a.ownerID = playerID;
			a.isEndAction = true;
			return a;
		}
	};
}