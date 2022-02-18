#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameInfo.h>
#pragma warning(disable: 5045)
namespace SGA
{
	bool Entity::isNeutral() const
	{
		return ownerID == Player::getNeutralPlayerID();
	}
	
	int Entity::getEntityTypeID() const
	{
		return type->getID();
	}

	void Entity::init(const EntityType* newType, int entityID)
	{
		this->type = newType;
		this->id = entityID;
		
		// Add actions
		attachedActions.reserve(newType->getActionIDs().size());
		for (auto actionTypeID : newType->getActionIDs())
		{
			attachedActions.emplace_back(ActionInfo{ actionTypeID, 0 });
		}

		// Set parameter values
		//lineOfSightRange = type->getLoSRange();
		///*parameters.reserve(type->getParameters().size());*/
		//maxParameters.reserve(type->getParameters().size());
		//minParameters.reserve(type->getParameters().size());
		for (const auto& idParamPair : type->getParameters())
		{
			parameters[idParamPair.second.getIndex()]=idParamPair.second.getDefaultValue();
			maxParameters[idParamPair.second.getIndex()]=idParamPair.second.getMaxValue();
			minParameters[idParamPair.second.getIndex()]=idParamPair.second.getMinValue();
		}
	}

	double Entity::getParameter(const std::string& paramName) const
    {
       for(const auto& param : type->getParameters()) {
          if(param.second.getName() == paramName) {
             return parameters.find(param.second.getIndex())->second;
          }
       }
	   return -1;
    }

	double& Entity::getRawParameter(const std::string& paramName)
	{
		for (const auto& param : type->getParameters())
		{
			if (param.second.getName() == paramName)
			{
				return parameters[param.second.getIndex()];
			}
		}

		throw std::runtime_error("Parameter not found");
	}


	std::vector<std::string> Entity::getEntityParameterNames() const
	{
		std::vector<std::string> paramNames;
		for (const auto& param : type->getParameters())
			paramNames.emplace_back(param.second.getName());

		return paramNames;
	}


	std::unordered_map<std::string, double> Entity::getEntityParameters() const
	{
		std::unordered_map<std::string, double> params;
		for (const auto& param : type->getParameters())
			params.emplace(param.second.getName(), parameters.find(param.second.getIndex())->second);
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


	std::vector<ActionType> Entity::getActionTypes(const GameInfo& gameInfo) const
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
		std::cout << "[" << type->getName() << "],";
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
		for (auto& parameterType : type->getParameters())
		{
			std::cout << "(" << parameterType.second.getName() << ": " << getParameter(parameterType.second.getName()) << ")";
		}

		std::cout << "]" << std::endl;
	}

	void Entity::recomputeStats()
	{
		//Remove buffs applied but keep value clamped to min and max
		for (size_t i = 0; i < (size_t)parameters.size(); i++)
		{
			const auto& param = type->getParameterByIndex(static_cast<int>(i));
			double maxParameter = param.getMaxValue();
			double minParameter = param.getMinValue();

			//Update the max value
			maxParameters[i] = maxParameter;

			//Keep parameter inside max and min
			if (parameters[i] > maxParameter)
				parameters[i] = maxParameter;
			else if (parameters[i] < minParameter)
				parameters[i] = minParameter;

		}

		//Recompute each parameter
		for (size_t i = 0; i < (size_t)parameters.size(); i++)
		{
			const auto& param = type->getParameterByIndex(static_cast<int>(i));
			double previousMaxParameter = param.getMaxValue();
			double maxParameter = previousMaxParameter;

			//Add to each parameterMax the additive and multiplication:	
			//Add buffs additive
			for (auto& buff : buffs)
			{
				const auto& buffType = buff.getType();
				maxParameter = buffType.getParameterWithAdditiveBuffsApplied(maxParameter, param.getID());
			}

			double multiplicationSum = 0;
			//Add buffs multiplication
			for (auto& buff : buffs)
			{
				auto& buffType = buff.getType();
				multiplicationSum += buffType.getMultiplicationSum(previousMaxParameter, param.getID());
			}
			maxParameter += multiplicationSum;

			//Write new value with the different of the max parameters
			parameters[i] += maxParameter - previousMaxParameter;
			//Update the max value
			maxParameters[i] = maxParameter;
		}
	}
}
