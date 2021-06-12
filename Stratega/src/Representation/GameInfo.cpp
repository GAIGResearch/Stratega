#include <Stratega/Representation/GameInfo.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	GameDescription& GameInfo::getGameDescription() const
	{
		return *gameDescription;
	}
	
	const EntityType& GameInfo::getEntityType(int entityTypeID) const
	{
		auto it = entityTypes->find(entityTypeID);
		if (it != entityTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("ERROR: No entity type associated to ID " + std::to_string(entityTypeID));
	}

	const ActionType& GameInfo::getActionTypeConst(int actionTypeID)
	{
		auto it = actionTypes->find(actionTypeID);
		if (it != actionTypes->end())
		{
			return it->second;
		}
		else throw std::runtime_error("ERROR: No action type associated to ID " + std::to_string(actionTypeID));
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
			if (element.second.name == actionName)
				return element.second.id;
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
		else throw std::runtime_error("ERROR: No player parameter associated to ID " + std::to_string(id));
	}

	const Parameter& GameInfo::getParameterType(int entityTypeID, int globalParameterID) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		return entityType.parameters.find(globalParameterID)->second;
	}

	bool GameInfo::checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		for (const auto& parameter : entityType.parameters)
		{
			if (parameter.second.name == parameterName)
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
}