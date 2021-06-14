#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{
	bool Entity::isNeutral() const
	{
		return ownerID == Player::NEUTRAL_PLAYER_ID;
	}
	
	int Entity::getEntityTypeID() const
	{
		return type->id;
	}

	void Entity::init(const EntityType* type, int entityID)
	{
		this->type = type;
		this->id = entityID;
		
		// Add actions
		attachedActions.reserve(type->actionIds.size());
		for (auto actionTypeID : type->actionIds)
		{
			attachedActions.emplace_back(ActionInfo{ actionTypeID, 0 });
		}

		// Set parameter values
		lineOfSightRange = type->lineOfSightRange;
		parameters.reserve(type->parameters.size());
		for (const auto& idParamPair : type->parameters)
		{
			parameters.emplace_back(idParamPair.second.defaultValue);
		}
	}

	double Entity::getParameter(std::string paramName) const
	{
		for (const auto& param : type->parameters)
		{
			if (param.second.name == paramName)
			{
				return parameters[param.second.index];
			}
		}
	}


	std::vector<std::string> Entity::getEntityParameterNames() const
	{
		std::vector<std::string> paramNames;
		for (const auto& param : type->parameters)
			paramNames.emplace_back(param.second.name);

		return paramNames;
	}


	std::unordered_map<std::string, double> Entity::getEntityParameters() const
	{
		std::unordered_map<std::string, double> params;
		for (const auto& param : type->parameters)
			params.emplace(param.second.name, parameters[param.second.index]);
		return params;
	}


	void Entity::setActionTicks(int actionTypeID, int tick)
	{
		// ToDo We should probably find a way to avoid this loop (why attachedActions is not a map?)
		for (auto& actionInfo : attachedActions)
		{
			if (actionInfo.actionTypeID == actionTypeID)
			{
				actionInfo.lastExecutedTick = tick;
				return;
			}
		}
	}

	const ActionInfo& Entity::getActionInfo(int actionTypeID) const
	{
		for (auto& actionInfo : attachedActions)
		{
			if (actionInfo.actionTypeID == actionTypeID)
				return actionInfo;
		}

		throw std::runtime_error("No action type associated to ID " + std::to_string(actionTypeID));
	}


	const std::vector<ActionType> Entity::getActionTypes(const GameInfo& gameInfo) const
	{
		std::vector<ActionType> aTypes;
		for (const ActionInfo aInfo : attachedActions)
		{
			ActionType at = gameInfo.getActionType(aInfo.actionTypeID);
			aTypes.emplace_back(at);
		}

		return aTypes;
	}

	void Entity::printInfo() const
	{
		std::cout << "[" << type->name << "],";
		std::cout << " [ID: " << id << "],";
		std::cout << " [OwnerID: " << ownerID << "],";
		std::cout << " [Position: " << position.x << "," << position.y << "]";

		int parameterID = 0;
		if (parameters.empty())
		{
			std::cout << std::endl;	
			return;
		}

		std::cout << ", [Parameters: ";
		for (auto& parameter : parameters)
		{
			std::cout << "(" << type->parameters.find(parameterID++)->second.name << ": " << parameter << ")";
		}

		std::cout << "]" << std::endl;
	}

}
