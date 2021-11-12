#include <Stratega/Representation/EntityType.h>
#include <Stratega/ForwardModel/Action.h>
const SGA::Parameter& SGA::EntityType::getParameter(ParameterID parameterID) const
{
	auto it = parameters.find(parameterID);
	if (it != parameters.end())
	{
		return it->second;
	}
	else throw std::runtime_error("No parameter associated to ID " + std::to_string(id) + " in entity type " + name);
}

const SGA::Parameter& SGA::EntityType::getParameterByIndex(int index) const
{
	for (const auto& parameter : parameters)
	{
		if (parameter.second.getIndex() == index)
			return parameter.second;
	}

	throw std::runtime_error("No parameter associated to index " + std::to_string(id) + " in entity type " + name);
}

const SGA::Parameter& SGA::EntityType::getParameterByName(std::string& paramName) const
{
	for (const auto& parameter : parameters)
	{
		if (parameter.second.getName() == paramName)
			return parameter.second;
	}

	throw std::runtime_error("No parameter associated to index " + std::to_string(id) + " in entity type " + name);
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
		if (param.second.getName() == paramName)
		{
			return param.second.getMaxValue();
		}
	}
	return 0;
}


double SGA::EntityType::getParamMin(std::string paramName) const
{
	for (const auto& param : parameters)
	{
		if (param.second.getName() == paramName)
		{
			return param.second.getMinValue();
		}
	}
	return 0;
}