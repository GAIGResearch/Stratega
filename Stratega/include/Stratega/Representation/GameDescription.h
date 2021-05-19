#pragma once
#include <unordered_map>
#include <vector>
#include <Stratega/ForwardModel/ActionType.h>
namespace SGA
{
	//Forward declaration
	struct GameInfo;

	/// <summary>
	/// Represents a grouping of action types into categories.
	/// Can be used by the agent to query <see cref="SGA::ActionType"/> grouped in different categories.
	/// </summary>
	enum class ActionCategory
	{
		Move,
		Attack,
		Heal,
		Gather,
		Spawn,
		Research		
	};

	enum class EntityCategory
	{
		Base,
		Building,
		Spawner,
		Unit,
		NoFighter,
		Fighter,
		Melee,
		Ranged
	};


	struct GameDescription
	{
		GameDescription(std::unordered_map<ActionCategory, std::vector<int>> actionCat, 
						std::unordered_map<EntityCategory, std::vector<int>> entityCat):
			actionCategories(actionCat),
			entityCategories(entityCat)
		{
			
		}
		
		//List of action IDs grouped by action categories
		std::unordered_map<ActionCategory, std::vector<int>> actionCategories;

		//List of entity IDs grouped by entity categories
		std::unordered_map<EntityCategory, std::vector<int>> entityCategories;

		/// <summary>
		/// Returns all the actiontypes IDs of the selected action category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <returns>List of action type IDs.</returns>
		const std::vector<int> getActionTypesIDs(ActionCategory category) const;

		/// <summary>
		/// Returns all the actiontypes IDs of the selected action category.
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
