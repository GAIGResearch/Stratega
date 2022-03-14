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
		Null, //Default value, not used in YAML.
		Move,
		Attack,
		Heal,
		Gather,
		Spawn,
		Research,
		Empty
	};

	enum class EntityCategory
	{
		Null, //Default value, not used in YAML.
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

	private:

		//List of action IDs grouped by action categories
		std::unordered_map<ActionCategory, std::vector<int>> actionCategories;

		//List of entity IDs grouped by entity categories
		std::unordered_map<EntityCategory, std::vector<int>> entityCategories;


	public:

		GameDescription(std::unordered_map<ActionCategory, std::vector<int>> actionCat, 
						std::unordered_map<EntityCategory, std::vector<int>> entityCat):
			actionCategories(actionCat),
			entityCategories(entityCat)
		{
			
		}

		/// <summary>
		/// Returns the action categories of this game.
		/// </summary>
		const std::unordered_map<ActionCategory, std::vector<int>>& getActionCategories() const { return actionCategories; };

		/// <summary>
		/// Returns the entity categories of this game.
		/// </summary>
		const std::unordered_map<EntityCategory, std::vector<int>>& getEntityCategories() const { return entityCategories; };
			
		/// <summary>
		/// Returns all the actiontypes IDs of the selected action category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <returns>List of action type IDs.</returns>
		std::vector<int> getActionTypesIDs(ActionCategory category) const;


		/// <summary>
		/// Returns all the entity type IDs of the selected entity category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <returns>List of entity type IDs.</returns>
		std::vector<int> getEntityTypesIDs(EntityCategory category) const;


		/// <summary>
		/// Returns all the actiontypes IDs of the selected action category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <param name="gameInfo">The gameInfo object wich contains all the type of actions.</param>
		/// <returns>List of action types</returns>
		std::vector<ActionType> getActionTypes(ActionCategory category, const GameInfo& gameInfo) const;

		/// <summary>
		/// Returns all the entity type  IDs of the selected entity category.
		/// </summary>
		/// <param name="category">The category group to be searched.</param>
		/// <param name="gameInfo">The gameInfo object wich contains all the type of actions.</param>
		/// <returns>List of entity types</returns>
		std::vector<EntityType> getEntityTypes(EntityCategory category, const GameInfo& gameInfo) const;



		/// <summary>
		/// Checks if a given entity type ID belongs to a given category
		/// </summary>
		/// <param name="category">The category group to be cheked.</param>
		/// <param name="entityTypeId">ID Type of the entity that is to be checekd if it's part of the category.</param>
		/// <returns>True if entityTypeId is a subset of the category given</returns>
		bool isFromCategory(EntityCategory category, int entityTypeId) const;


		static std::string toString(ActionCategory ac)
		{
			switch (ac)
			{
				case ActionCategory::Null: return "None";
				case ActionCategory::Move: return "Move";
				case ActionCategory::Attack: return "Attack";
				case ActionCategory::Heal: return "Heal";
				case ActionCategory::Gather: return "Gather";
				case ActionCategory::Spawn: return "Spawn";
				case ActionCategory::Research: return "Research";
				case ActionCategory::Empty: return "Empty";
				default: return "Undefined";
			}
		}


		static std::string toString(EntityCategory ec)
		{
			switch (ec)
			{
				case EntityCategory::Null: return "None";
				case EntityCategory::Base: return "Base";
				case EntityCategory::Building: return "Building";
				case EntityCategory::Spawner: return "Spawner";
				case EntityCategory::Unit: return "Unit";
				case EntityCategory::NoFighter: return "NoFighter";
				case EntityCategory::Fighter: return "Fighter";
				case EntityCategory::Melee: return "Melee";
				case EntityCategory::Ranged: return "Ranged";
				default: return "Undefined";
			}
		}		


		//TODO Utilities
		//getClosestMoveActionTo(Position)
		//getEntitiesWithLowHP()
		//getMoveCloseActionToPoint
		//getMoveAwayFromPoint
		//getEntitiesWithLowHealth
	};
}
