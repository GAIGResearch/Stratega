#include <Stratega/Representation/Player.h>
namespace SGA
{
	bool Player::canExecuteAction(int actionTypeID) const
	{
		for (const auto& actionType : attachedActions)
		{
			if (actionType.actionTypeID == actionTypeID)
				return true;
		}
		return false;
	}

	const ActionInfo& Player::getActionInfo(int actionTypeID) const
	{
		for (const auto& actionInfo : attachedActions)
		{
			if (actionInfo.actionTypeID == actionTypeID)
				return actionInfo;
		}

		throw std::runtime_error("Tried accessing action with unknown actionType");
	}
}
