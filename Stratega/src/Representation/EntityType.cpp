#include <Representation/EntityType.h>

const SGA::Parameter& SGA::EntityType::getParameter(ParameterID id) const
{
	auto it = parameters.find(id);
	if (it != parameters.end())
	{
		return it->second;
	}
	else
	{
		std::string s;
		s.append("Tried accessing unknown parameter ID ");
		s.append(std::to_string(id));
		s.append("in entityType ");
		s.append(name);
		throw std::runtime_error(s);
	}
}

bool SGA::EntityType::canExecuteAction(int actionTypeID) const
{
	for (const auto& id : actionIds)
	{
		if (id == actionTypeID)
			return true;
	}
	return false;
}

SGA::Entity SGA::EntityType::instantiateEntity(int entityID) const
{
	Entity instance;
	instance.id = entityID;
	instance.typeID = this->id;
	instance.actionTypeIds = actionIds;

	// Set parameter values
	instance.parameters.reserve(parameters.size());
	for(const auto& idParamPair: parameters)
	{
		instance.parameters.emplace_back(idParamPair.second.defaultValue);
	}

	return instance;
}