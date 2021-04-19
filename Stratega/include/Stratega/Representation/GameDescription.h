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

	/// <summary>
	/// A collection of action to categories in a game.
	/// Gives information to the player about the type of action.
	/// </summary>
	struct GameDescription
	{
		GameDescription(std::unordered_map<ActionCategory, std::vector<int>> actionTypes):
			actionTypes(actionTypes)
		{
			
		}
		
		//List of action IDs group by action categories
		std::unordered_map<ActionCategory, std::vector<int>> actionTypes;

		/// <summary>
		/// Returns all the actiontypes IDs of the selected category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <returns>List of action type IDs.</returns>
		const std::vector<int> getActionTypesIDs(ActionCategory category) const;

		/// <summary>
		/// Returns all the actiontypes IDs of the selected category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <param name="gameInfo">The gameInfo object wich contains all the type of actions.</param>
		/// <returns>List of action types</returns>
		const std::vector<ActionType> getActionTypes(ActionCategory category, const GameInfo& gameInfo) const;

		//TODO Utilities
		//getClosestMoveActionTo(Position)
		//getEntitiesWithLowHP()
		//getMoveCloseActionToPoint
		//getMoveAwayFromPoint
		//getEntitiesWithLowHealth
	};
}
