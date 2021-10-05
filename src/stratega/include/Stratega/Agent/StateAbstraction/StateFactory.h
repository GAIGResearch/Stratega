#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/Agent/StateAbstraction/AbstractState.h>
#include <string>
#include <iostream>
#include <map>
#include <Stratega/Utils/contains.h>

namespace SGA
{
	struct StateFactoryConfiguration {
		bool insertMap = false;
		bool insertEntityPositions = true;
		//std::map<std::string, bool> insertPlayerParameters;
		std::map<std::string, bool> insertEntityParameters;
		//std::map<std::string, std::map<int, int>> mapParameterNameToUnitTypeParameterID;

		StateFactoryConfiguration() {

		}
	};

	class StateFactory
	{
	public:
		StateFactoryConfiguration config;

	public:
		StateFactory(StateFactoryConfiguration _config) : config(_config)
		{

		}

		StateFactory(GameState& tbs)
		{
			//bool flip = true;
			config = StateFactoryConfiguration();
			const auto& parameters = tbs.getGameInfo()->getParameterIDLookup();
			for (auto entry : parameters) {
				if (config.insertEntityParameters.count(entry.first)==0)
				//if (!config.insertEntityParameters.contains(entry.first))
				{
					config.insertEntityParameters[entry.first] = true;
					//config.insertEntityParameters[entry.first] = flip;
					//flip = !flip;
				}
			}
		}

		AbstractState createAbstractState(GameState& tbsState);

		friend std::ostream& operator<<(std::ostream& os, const StateFactory& dt)
		{
			os << "Map: " << dt.config.insertMap << std::endl;
			os << "Positions: " << dt.config.insertEntityPositions << std::endl;
			for (auto entry : dt.config.insertEntityParameters)
			{
				os << entry.first << ": " << entry.second << std::endl;
			}

			return os;
		}
	};
}