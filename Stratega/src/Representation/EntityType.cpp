#include <Stratega/Representation/EntityType.h>
#include <Stratega/ForwardModel/Action.h>
const SGA::Parameter& SGA::EntityType::getParameter(ParameterID parameterID) const
{
	auto it = parameters.find(parameterID);
	if (it != parameters.end())
	{
		return it->second;
	}
	else
	{
		std::string s;
		s.append("Tried accessing unknown parameter ID ");
		s.append(std::to_string(parameterID));
		s.append("in entityType ");
		s.append(name);
		throw std::runtime_error(s);
	}
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
	Entity instance(this);
	instance.init(entityID);
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