#include <Stratega/Representation/TileType.h>

const SGA::Parameter& SGA::TileType::getParameter(ParameterID parameterID) const
{
	auto it = parameters.find(parameterID);
	if (it != parameters.end())
	{
		return it->second;
	}
	else throw std::runtime_error("No parameter associated to ID " + std::to_string(id) + " in tile type " + name);
}

const SGA::Parameter& SGA::TileType::getParameterByIndex(int index) const
{
	for (const auto& parameter : parameters)
	{
		if (parameter.second.getIndex() == index)
			return parameter.second;
	}

	throw std::runtime_error("No parameter associated to index " + std::to_string(id) + " in tile type " + name);
}

const SGA::Parameter& SGA::TileType::getParameterByName(std::string& paramName) const
{
	for (const auto& parameter : parameters)
	{
		if (parameter.second.getName() == paramName)
			return parameter.second;
	}

	throw std::runtime_error("No parameter associated to index " + std::to_string(id) + " in tile type " + name);
}

double SGA::TileType::getParamMax(std::string paramName) const
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


double SGA::TileType::getParamMin(std::string paramName) const
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