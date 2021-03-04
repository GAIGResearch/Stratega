#pragma once
#include <Stratega/Representation/Parameter.h>
#include <Stratega/ForwardModel/Action.h>
#include <unordered_map>
#include <vector>

namespace SGA
{
	struct Player
	{
		static const int NEUTRAL_PLAYER_ID = -1;
		
		int id;
		double score;
		bool canPlay;
		std::vector<double> parameters;
		std::vector<Action> continuousAction;

		std::vector<ActionInfo> attachedActions;

		bool canExecuteAction(int actionTypeID) const
		{
			for (const auto& actionType : attachedActions)
			{
				if (actionType.actionTypeID == actionTypeID)
					return true;
			}
			return false;
		}

		const ActionInfo& getActionInfo(int actionTypeID) const
		{
			for (auto& actionInfo : attachedActions)
			{
				if (actionInfo.actionTypeID == actionTypeID)
					return actionInfo;
			}
		}
	};
}
