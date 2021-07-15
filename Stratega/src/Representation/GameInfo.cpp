#include <Stratega/Representation/GameInfo.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	const EntityType& GameInfo::getEntityType(int entityTypeID) const
	{
		auto it = entityTypes->find(entityTypeID);
		if (it != entityTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("No entity type associated to ID " + std::to_string(entityTypeID));
	}

	const ActionType& GameInfo::getActionTypeConst(int actionTypeID)
	{
		auto it = actionTypes->find(actionTypeID);
		if (it != actionTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("No action type associated to ID " + std::to_string(actionTypeID));
	}

	int GameInfo::getParameterGlobalID(std::string parameterName)
	{
		int foundId = -1;
		auto  iterator = parameterIDLookup->find(parameterName);

		if (iterator != parameterIDLookup->end())
			foundId = iterator->second;

		return foundId;
	}

	int GameInfo::getActionTypeID(std::string actionName)
	{
		int foundId = -1;
		for (const auto& element : *actionTypes)
		{
			if (element.second.getName() == actionName)
				return element.second.getID();
		}

		return foundId;
	}

	const SGA::Parameter& GameInfo::getPlayerParameter(ParameterID id) const
	{
		auto it = playerParameterTypes->find(id);
		if (it != playerParameterTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("No player parameter associated to ID " + std::to_string(id));
	}

	const Parameter& GameInfo::getParameterType(int entityTypeID, int globalParameterID) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		return entityType.getParameters().find(globalParameterID)->second;
	}

	bool GameInfo::checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		for (const auto& parameter : entityType.getParameters())
		{
			if (parameter.second.getName() == parameterName)
				return true;
		}

		return false;
	}

	const ActionType& GameInfo::getActionType(int typeID) const
	{
		return actionTypes->find(typeID)->second;
	}

	const TileType& GameInfo::getTileType(int tileTypeID) const
	{
		return tileTypes->find(tileTypeID)->second;
	}

	std::unordered_map<int, int> GameInfo::getTechnologyCounts() const
	{
		std::unordered_map<int, int> counts;
		for (const auto& [id, techTree] : technologyTreeCollection->technologyTreeTypes)
		{
			counts.emplace(id, techTree.technologies.size());
		}
		return counts;
	}


	const TechnologyTreeNode& GameInfo::getTechnology(int technologyID) const 
	{
		return technologyTreeCollection->getTechnology(technologyID);
	}

	int GameInfo::getTechnologyTypeID(const std::string& technologyName) const
	{
		return technologyTreeCollection->getTechnologyTypeID(technologyName);
	}

	std::vector<TechnologyTreeNode> GameInfo::getTreeNodes(int techTreeID) const
	{
		std::vector<TechnologyTreeNode> techs;

		const auto& it = technologyTreeCollection->technologyTreeTypes.find(techTreeID);
		if (it != technologyTreeCollection->technologyTreeTypes.end())
		{
			std::unordered_map<int, TechnologyTreeNode> ttNodes = it->second.technologies;
			for (const auto& [id, treeNode] : ttNodes)
			{
				techs.emplace_back(treeNode);
			}
		}

		return techs;
	}

	std::vector<int> GameInfo::getTechTreesIDs() const
	{
		std::vector<int> ids;
		for (const auto& [id, techTree] : technologyTreeCollection->technologyTreeTypes)
		{
			ids.emplace_back(id);
		}
		return ids;
	}
		

}