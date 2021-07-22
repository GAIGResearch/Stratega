
#include <Stratega/Representation/GameDescription.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{

	std::vector<int> GameDescription::getEntityTypesIDs(EntityCategory category) const
	{
		auto it = entityCategories.find(category);
		if (it != entityCategories.end())
			return it->second;
		else
			return std::vector<int>();
	}


	std::vector<int> GameDescription::getActionTypesIDs(ActionCategory category) const
	{
		auto it = actionCategories.find(category);
		if (it != actionCategories.end())
			return it->second;
		else
			return std::vector<int>();
	}

	std::vector<ActionType> GameDescription::getActionTypes(ActionCategory category, const GameInfo& gameInfo) const
	{
		std::vector<ActionType> actionTypes;
		auto it = actionCategories.find(category);
		if (it != actionCategories.end())
		{
			for (auto& actionTypeID : it->second)
			{
				actionTypes.emplace_back(gameInfo.getActionType(actionTypeID));
			}
		}		

		return actionTypes;
	}


	std::vector<EntityType> GameDescription::getEntityTypes(EntityCategory category, const GameInfo& gameInfo) const
	{
		std::vector<EntityType> entityTypes;
		auto it = entityCategories.find(category);
		if (it != entityCategories.end())
		{
			for (auto& entityTypeID : it->second)
			{
				entityTypes.emplace_back(gameInfo.getEntityType(entityTypeID));
			}
		}

		return entityTypes;
	}

	bool GameDescription::isFromCategory(EntityCategory category, int entityTypeId) const
	{
		auto entityTypesIt = entityCategories.find(category);
		if (entityTypesIt != entityCategories.end())
		{
			auto types = entityTypesIt->second;
			return std::find(types.begin(), types.end(), entityTypeId) != types.end();
		}
		return false;		
	}

}
