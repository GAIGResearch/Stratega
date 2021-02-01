#pragma once
#include <vector>
#include <ForwardModel/ActionTarget.h>

namespace SGA
{
	struct ActionInfo
	{
		int actionTypeID;
		int lastExecutedTick;
	};
	
	class EntityForwardModel;
	
	enum ActionFlag
	{
		None = 1 << 0,
		EndTickAction = 1 << 1,
		ContinuousAction = 1 << 2,
		AbortContinuousAction = 1 << 3
	};
	
	struct Action
	{
		Action():
			actionTypeID(-1),
			ownerID(0),
			elapsedTicks(0),
			continuousActionID(-1),
			actionTypeFlags(None)
		{
			
		}
		
		ActionFlag actionTypeFlags;
		
		int actionTypeID;
		// Contains all targets involved in an action
		// UnitAction: Index 0 contains the source and Index 1 the target of the action//opposite
		// PlayerAction": Index 0 contains the target of the action
		std::vector<ActionTarget> targets;
		int ownerID;

		int continuousActionID;
		int elapsedTicks;
		
		void execute(GameState& state, const EntityForwardModel& fm) const;

		static Action createEndAction(int playerID)
		{
			Action a;
			a.actionTypeID = -1;
			a.actionTypeFlags = EndTickAction;
			a.ownerID = playerID;
			return a;
		}

		static Action createAbortAction(int playerID,int entityID, int continuousActionID)
		{
			Action a;			
			a.ownerID = playerID;			
			a.actionTypeFlags = AbortContinuousAction;
			a.continuousActionID = continuousActionID;
			a.targets.emplace_back(ActionTarget::createEntityActionTarget(entityID));
			a.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(continuousActionID));
			return a;
		}
		
		static Action createAbortAction(int playerID, int continuousActionID)
		{
			Action a;
			a.ownerID = playerID;
			a.actionTypeFlags = AbortContinuousAction;
			a.continuousActionID = continuousActionID;
			a.targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
			a.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(continuousActionID));
			return a;
		}
	};
}