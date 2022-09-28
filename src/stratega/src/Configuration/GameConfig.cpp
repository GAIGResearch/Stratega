#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <Stratega/Representation/LevelDefinition.h>
namespace SGA
{
	std::vector<std::unique_ptr<Agent>> GameConfig::generateAgents() const
	{
		std::vector<std::unique_ptr<Agent>> agents;
		for (const auto& agentNode : agentParams)
		{
			if (agentNode.second.IsNull())
			{
				agents.emplace_back(AgentFactory::get().createAgent(agentNode.first));
			}
			else
			{
				agents.emplace_back(AgentFactory::get().createAgent(agentNode.first, agentNode.second));
			}
		}
		return agents;
	}

	int GameConfig::addPlayer(GameState& state, GameInfo& gameInfo) const
	{
		int nextPlayerID = state.getNumPlayers();
		Player player = { nextPlayerID, true };

		// Add parameters
		player.resizeParameters(static_cast<int>(gameInfo.getPlayerParameterTypes().size()));
		const auto parameterTypes = gameInfo.getPlayerParameterTypes();
		for (const auto& idParamPair : parameterTypes)
		{
			player.setParameter(idParamPair.second.getIndex(), idParamPair.second.getDefaultValue());
			player.setMaxParameter(idParamPair.second.getIndex(), idParamPair.second.getMaxValue());
			player.setMinParameter(idParamPair.second.getIndex(), idParamPair.second.getMinValue());
		}

		// Add actions
		player.resAttachedActions(static_cast<int>(playerActionIds.size()));
		for (auto actionTypeID : playerActionIds)
		{
			player.addAttachedAction(actionTypeID, 0);
		}

		state.addPlayer(player);
		return player.getID();
	}

	std::unique_ptr<GameState> GameConfig::generateGameState(int levelID) const
	{
		// Initialize state
		std::unique_ptr<GameState> state = std::make_unique<GameState>();

		initializeGamestate(*state);

		generateGameInfo(*state);
		
		// Add parameters
		state->resizeParameters(static_cast<int>(state->getGameInfo()->getStateParameterTypes().size()));
		const auto parameterTypes = state->getGameInfo()->getStateParameterTypes();
		for (const auto& idParamPair : parameterTypes)
		{
			state->setParameter(idParamPair.second.getIndex(), idParamPair.second.getDefaultValue());
			state->setMaxParameter(idParamPair.second.getIndex(), idParamPair.second.getMaxValue());
			state->setMinParameter(idParamPair.second.getIndex(), idParamPair.second.getMinValue());
		}

		addPlayers(*state);

		// Create some lookups for initializing the board and entities
		createTileLookup(*state);
		createEntitiesLookup(*state);
		
		//Switch to selected level
		int mapIDtoLoad = selectedLevel;

		if (levelID != -1)
			mapIDtoLoad = levelID;

		auto selectedLevelDefinition = levelDefinitions.find(mapIDtoLoad);

        /*
        for (auto it = levelDefinitions.begin(); it != levelDefinitions.end(); it++)
        {
            std::cout << it->first    // string (key)
                      << ':'
                      << it->second.name   // string's value 
                      << std::endl;
        }*/

		if(selectedLevelDefinition==levelDefinitions.end())
		{
            std::cout<<"Selected level definition not found"<<std::endl;
			throw std::runtime_error("Selected level definition not found");
		}

		auto& board = selectedLevelDefinition->second.board;

		std::vector<Tile> tiles=instanceTiles(*state, board);
		instanceEntities(*state, selectedLevelDefinition->second.entityPlacements);

		initBoard(*state, tiles, board);

		// Initialize Pathfinding
		if(gameType == GameType::RTS)
		{
			auto* rtsFM = dynamic_cast<SGA::RTSForwardModel*>(forwardModel.get());
			rtsFM->buildNavMesh(*state, NavigationConfig{});
		}

		initResearchTechs(*state);
		
		return state;
	}
	
	size_t GameConfig::getNumberOfPlayers() const
	{
		return numPlayers == -1 ? agentParams.size() : static_cast<size_t>(numPlayers);
	}

	int GameConfig::getEntityID(const std::string& name) const
	{
		for (const auto& idTypePair : entityTypes)
		{
			if (idTypePair.second.getName() == name)
				return idTypePair.first;
		}

		throw std::runtime_error("Unknown entity with name " + name);
	}

	int GameConfig::getActionID(const std::string& name) const
	{
		for (const auto& idTypePair : actionTypes)
		{
			if (idTypePair.second.getName() == name)
				return idTypePair.first;
		}

		throw std::runtime_error("Unknown action with name " + name);
	}

	int GameConfig::getTechnologyID(const std::string& name) const
	{
		for (const auto& treeType: technologyTreeCollection.technologyTreeTypes)
		{
			for (const auto& idTypePair : treeType.second.technologies)
			{
				if (idTypePair.second.name == name)
					return idTypePair.second.id;
			}
		}			

		throw std::runtime_error("Unknown Technolgy with name " + name);
	}
	
	int GameConfig::getTileID(const std::string& name) const
	{
		for (const auto& idTypePair : tileTypes)
		{
			if (idTypePair.second.getName() == name)
				return idTypePair.first;
		}

		throw std::runtime_error("Unknown tile with name " + name);
	}
}