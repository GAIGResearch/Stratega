#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#pragma warning(disable: 5045)
namespace SGA
{
	AbstractState StateFactory::createAbstractState(GameState& tbsState)
	{
		AbstractState state = AbstractState();
		std::vector<double> stateInfo = { static_cast<double>(tbsState.getCurrentTBSPlayer()), static_cast<double>(tbsState.getCurrentTick()) };
		state.addAttribute("StateInfo", stateInfo);

		if (config.insertMap)
		{
			std::vector<double> mapDescription;
			for (int i = 0; i < tbsState.getBoardWidth(); i++)
			{
				for (int j = 0; j < tbsState.getBoardHeight(); j++)
				{
					const auto& tile = tbsState.getTileAt(Vector2i(i, j));
					mapDescription.push_back(tile.getTileTypeID());
				}
			}
			state.addAttribute("Map", mapDescription);
		}

		if (config.insertEntityPositions) 
		{
			std::vector<double> positions;
			for (auto entity : tbsState.getEntities()) {
				positions.push_back(entity.x());
				positions.push_back(entity.y());
			}
			state.addAttribute("EntityPositions", positions);
		}

		for (auto entry : config.insertEntityParameters)
		{
			if (entry.second == true) {
				std::vector<double> parameterValues;
				//int globalParameterID = tbsState.getParameterGlobalID(entry.first);
				for (auto entity : tbsState.getEntities()) {
					const auto& entityType = tbsState.getGameInfo()->getEntityType(entity.getEntityTypeID());
					for (const auto& parameter : entityType.getParameters())
					{
						if (parameter.second.getName() == entry.first)
						{
							parameterValues.push_back(entity.getRawParameterAt(parameter.second.getIndex()));
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

