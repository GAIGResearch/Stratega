
#include <Stratega/Representation/GameDescription.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{
	const std::vector<int> GameDescription::getActionTypesIDs(ActionCategory category) const
	{
		auto it = actionTypes.find(category);
		if (it != actionTypes.end())
			return it->second;
		else
			return std::vector<int>();
	}

	const std::vector<ActionType> GameDescription::getActionTypes(ActionCategory category, const GameInfo& gameInfo) const
	{
		auto it = actionTypes.find(category);
		if (it != actionTypes.end())
		{
			std::vector<ActionType> actionTypes;
			for (auto& actionTypeID : it->second)
			{
				actionTypes.emplace_back(gameInfo.getActionType(actionTypeID));
			}

			return actionTypes;
		}			
		else
			return std::vector<ActionType>();
	}

}
