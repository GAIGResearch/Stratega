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

	const BuffType& GameInfo::getBuffType(int buffTypeID) const
	{
		auto it = buffTypes->find(buffTypeID);
           if(it != buffTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("No buff type associated to ID " + std::to_string(buffTypeID));
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
	
	const SGA::Parameter& GameInfo::getPlayerParameter(std::string& name) const
	{
		for (const auto& parameter : *playerParameterTypes)
		{
			if(parameter.second.getName()== name)
				return parameter.second;
		}

		throw std::runtime_error("No player parameter associated to name " + name);
	}
	
	const SGA::Parameter& GameInfo::getPlayerParameterByIndex(int index) const
	{
		for (const auto& parameter : *playerParameterTypes)
		{
			if(parameter.second.getIndex()==index)
				return parameter.second;
		}

		throw std::runtime_error("No player parameter associated to index " + std::to_string(index));
	}

	const SGA::Parameter& GameInfo::getStateParameter(ParameterID id) const
	{
		auto it = stateParameterTypes->find(id);
		if (it != stateParameterTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("No state parameter associated to ID " + std::to_string(id));
	}
	
	const SGA::Parameter& GameInfo::getStateParameter(std::string& name) const
	{
		for (const auto& parameter : *stateParameterTypes)
		{
			if(parameter.second.getName()== name)
				return parameter.second;
		}

		throw std::runtime_error("No state parameter associated to name " + name);
	}
	
	const SGA::Parameter& GameInfo::getStateParameterByIndex(int index) const
	{
		for (const auto& parameter : *stateParameterTypes)
		{
			if(parameter.second.getIndex()==index)
				return parameter.second;
		}

		throw std::runtime_error("No state parameter associated to index " + std::to_string(index));
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
		for (auto techTree : technologyTreeCollection->technologyTreeTypes)
		{
			counts.emplace(techTree.first, static_cast<int>(techTree.second.technologies.size()));
		}
		/*for (const auto& [id, techTree] : technologyTreeCollection->technologyTreeTypes)
		{
			counts.emplace(id, techTree.technologies.size());
		}*/
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

			for (auto node : ttNodes)
			{
				techs.emplace_back(node.second);
			}
			/*for (const auto& [id, treeNode] : ttNodes)
			{
				techs.emplace_back(treeNode);
			}*/
		}

		return techs;
	}

	std::vector<int> GameInfo::getTechTreesIDs() const
	{
		std::vector<int> ids;
		for (auto pair : technologyTreeCollection->technologyTreeTypes)
		{
			ids.emplace_back(pair.first);
		}
		/*for (const auto& [id, techTree] : technologyTreeCollection->technologyTreeTypes)
		{
			ids.emplace_back(id);
		}*/
		return ids;
	}
		

}