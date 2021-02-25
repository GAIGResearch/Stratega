#pragma once
#include <Stratega/Agent/StateAbstraction/StateFactory.h>

namespace SGA
{
	AbstractState StateFactory::createAbstractState(TBSGameState& tbsState)
	{
		AbstractState state = AbstractState();

		if (config.insertMap)
		{
			std::vector<double> mapDescription;
			for (int i = 0; i < tbsState.board.getWidth(); i++) 
			{
				for (int j = 0; j < tbsState.board.getHeight(); j++) 
				{
					auto tile = tbsState.board[Vector2i(i, j)];
					mapDescription.push_back(tile.tileTypeID);
				}
			}
			state.addAttribute("Map", mapDescription);
		}

		if (config.insertEntityPositions) 
		{
			std::vector<double> positions;
			for (auto entity : tbsState.entities) {
				positions.push_back(entity.position.x);
				positions.push_back(entity.position.y);
			}
			state.addAttribute("EntityPositions", positions);
		}

		for (auto entry : config.insertEntityParameters)
		{
			if (entry.second == true) {
				std::vector<double> parameterValues;
				//int globalParameterID = tbsState.getParameterGlobalID(entry.first);
				for (auto entity : tbsState.entities) {
					const auto& entityType = tbsState.getEntityType(entity.typeID);
					for (const auto& parameter : entityType.parameters)
					{
						if (parameter.second.name == entry.first)
						{
							parameterValues.push_back(entity.parameters[parameter.second.index]);
							break;
						}
					}
				}
				state.addAttribute(entry.first, parameterValues);
			}
		}
		

		return state;
	}
}

