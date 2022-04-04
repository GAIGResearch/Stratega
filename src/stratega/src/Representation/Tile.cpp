#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/TileType.h>
namespace SGA
{
	Tile::Tile(const TileType* newTileType, int x, int y) :
		type(newTileType),
		blockSight(false),
		walkable(true),
		position(x, y)
	{
		if(newTileType!=NULL)
			for (const auto& idParamPair : type->getParameters())
			{
				parameters[idParamPair.second.getIndex()] = idParamPair.second.getDefaultValue();
				maxParameters[idParamPair.second.getIndex()] = idParamPair.second.getMaxValue();
				minParameters[idParamPair.second.getIndex()] = idParamPair.second.getMinValue();
			}
	}
	int Tile::getTileTypeID() const
	{
		int tileTypeID = -1;
		if (type)
			tileTypeID = type->getID();

		return tileTypeID;
	}


	const std::string Tile::name() const {
	
		if(type != nullptr)
			return type->getName();
		return "Fog";
	}


	double Tile::getParameter(const std::string& paramName) const
	{
		for (const auto& param : type->getParameters()) {
			if (param.second.getName() == paramName) {

				auto it = parameters.find(param.second.getIndex());
				if (it != parameters.end())
					return it->second;
				else
					throw std::runtime_error("Parameter not found");
			}
		}
		return -1;
	}

	double& Tile::getRawParameter(const std::string& paramName)
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


	std::vector<std::string> Tile::getTileParameterNames() const
	{
		std::vector<std::string> paramNames;
		for (const auto& param : type->getParameters())
			paramNames.emplace_back(param.second.getName());

		return paramNames;
	}


	std::unordered_map<std::string, double> Tile::getTileParameters() const
	{
		std::unordered_map<std::string, double> params;
		for (const auto& param : type->getParameters())
		{
			auto it = parameters.find(param.second.getIndex());
			if (it != parameters.end())
				params.emplace(param.second.getName(), it->second);
			else
				throw std::runtime_error("Parameter not found");
		}


		return params;
	}
}