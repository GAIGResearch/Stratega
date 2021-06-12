#include <Stratega/Representation/EntityType.h>
#include <Stratega/ForwardModel/Action.h>
const SGA::Parameter& SGA::EntityType::getParameter(ParameterID parameterID) const
{
	auto it = parameters.find(parameterID);
	if (it != parameters.end())
	{
		return it->second;
	}
	else throw std::runtime_error("ERROR: No parameter associated to ID " + std::to_string(id) + " in entity type " + name);
}

bool SGA::EntityType::canExecuteAction(int actionTypeID) const
{
	for (const auto& actionID : actionIds)
	{
		if (actionID == actionTypeID)
			return true;
	}
	return false;
}

SGA::Entity SGA::EntityType::instantiateEntity(int entityID) const
{
	Entity instance;
	instance.init(this, entityID);
	return instance;
}

double SGA::EntityType::getParamMax(std::string paramName) const
{
	for (const auto& param : parameters)
	{
		if (param.second.name == paramName)
		{
			return param.second.maxValue;
		}
	}
	return 0;
}


double SGA::EntityType::getParamMin(std::string paramName) const
{
	for (const auto& param : parameters)
	{
		if (param.second.name == paramName)
		{
			return param.second.minValue;
		}
	}
	return 0;
}