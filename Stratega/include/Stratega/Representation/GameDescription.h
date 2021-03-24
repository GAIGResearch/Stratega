#pragma once
#include <unordered_map>
#include <vector>
#include <Stratega/ForwardModel/ActionType.h>
namespace SGA
{
	//Forward declaration
	class GameInfo;
	
	enum class ActionCategory
	{
		Move,
		Attack,
		Heal,
		Gather,
		Spawn,
		Research		
	};
	
	struct GameDescription
	{
		GameDescription(std::unordered_map<ActionCategory, std::vector<int>> actionTypes):
			actionTypes(actionTypes)
		{
			
		}
		//List of action IDs group by action categories
		std::unordered_map<ActionCategory, std::vector<int>> actionTypes;

		const std::vector<int> getActionTypesIDs(ActionCategory category) const;

		const std::vector<ActionType> getActionTypes(ActionCategory category, const GameInfo& state) const;

		//TODO Utilities
		//getClosestMoveActionTo(Position)
		//getEntitiesWithLowHP()
		//getMoveCloseActionToPoint
		//getMoveAwayFromPoint
		//getEntitiesWithLowHealth
	};
}
