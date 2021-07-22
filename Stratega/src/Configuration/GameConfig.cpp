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
		return std::move(agents);
	}

	int GameConfig::addPlayer(std::unique_ptr<GameState>& state, GameInfo& gameInfo) const
	{
		int nextPlayerID = state->getNumPlayers();
		Player player = { nextPlayerID, true };

		// Add parameters
		player.resizeParameters(gameInfo.getPlayerParameterTypes().size());
		const auto parameterTypes = gameInfo.getPlayerParameterTypes();
		for (const auto& idParamPair : parameterTypes)
		{
			player.setParameter(idParamPair.second.getIndex(), idParamPair.second.getDefaultValue());
		}

		// Add actions
		player.resAttachedActions(playerActionIds.size());
		for (auto actionTypeID : playerActionIds)
		{
			player.addAttachedAction(actionTypeID, 0);
		}

		state->addPlayer(player);
		return player.getID();
	}

	std::unique_ptr<GameState> GameConfig::generateGameState(int levelID) const
	{
		// Initialize state
		std::unique_ptr<GameState> state = std::make_unique<GameState>();
		state->setGameType(gameType);
		state->setTickLimit(tickLimit);
		state->setCurrentTBSPlayer(gameType == SGA::GameType::RTS ? -1 : 0);

		//GameInfo
		GameInfo gameInfo;		
		gameInfo.setEntityTypes(std::make_shared<std::unordered_map<int, EntityType>>(entityTypes));
		gameInfo.setTileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes));
		gameInfo.setPlayerParameterTypes(std::make_shared<std::unordered_map<ParameterID, Parameter>>(playerParameterTypes));
		gameInfo.setEntityGroups(entityGroups);
		gameInfo.setActionTypes(std::make_shared<std::unordered_map<int, ActionType>>(actionTypes));
		gameInfo.setParameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>(parameters));
		gameInfo.setTechnologyTreeCollection(std::make_shared<TechnologyTreeCollection>(technologyTreeCollection));
		gameInfo.setPlayerSpawnableTypes(std::make_shared<std::unordered_set<EntityTypeID>>(playerSpawnableTypes));
		gameInfo.setYAMLPath(yamlPath);
		gameInfo.setGameDescription(std::make_shared<GameDescription>(actionCategories, entityCategories));
		state->setGameInfo(std::make_shared<GameInfo>(gameInfo));

		
		std::unordered_set<int> playerIDs;
		for (auto i = 0; i < getNumberOfPlayers(); i++)
		{
			playerIDs.emplace(addPlayer(state, gameInfo));
		}

		// Create some lookups for initializing the board and entities
		std::unordered_map<char, const TileType*> tileLookup;
		const auto* defaultTile = &state->getGameInfo()->getTileTypes().begin()->second;
		const auto& tileTypes = state->getGameInfo()->getTileTypes();
		for(const auto& idTilePair : tileTypes)
		{
			tileLookup.emplace(idTilePair.second.getSymbol(), &idTilePair.second);
			if (idTilePair.second.isDefaultTile())
				defaultTile = &idTilePair.second;
		}

		std::unordered_map<char, const EntityType*> entityLookup;
		for(const auto& idEntityPair : state->getGameInfo()->getEntityTypes())
		{
			entityLookup.emplace(idEntityPair.second.getSymbol(), &idEntityPair.second);
		}

		// Configure board and spawn entities
		std::vector<Tile> tiles;
		
		//Switch to selected level
		int mapIDtoLoad = selectedLevel;
		if (levelID != -1)
			mapIDtoLoad = levelID;
		auto selectedLevelDefinition = levelDefinitions.find(mapIDtoLoad);
		if(selectedLevelDefinition==levelDefinitions.end())
		{
			throw std::runtime_error("Selected level definition not found");
		}

		auto& board = selectedLevelDefinition->second.board;

		//Instance Tiles
		for (size_t y = 0; y < board.getHeight(); y++)
		{
			for (size_t x = 0; x < board.getWidth(); x++)
			{
				tiles.emplace_back(board.get(x,y)->toTile(x, y));
			}
		}

		//Instance Entities
		for (auto& entity : selectedLevelDefinition->second.entityPlacements)
		{
			state->addEntity(*entity.entityType, entity.ownerID, entity.position);
		}
		
		//Initialize board with size and set of tiles.
		state->initBoard(board.getWidth(), tiles);

		// Initialize Pathfinding
		if(gameType == GameType::RTS)
		{
			auto* rtsFM = dynamic_cast<SGA::RTSForwardModel*>(forwardModel.get());
			rtsFM->buildNavMesh(*state, NavigationConfig{});
		}

		//Initialize researched list for all players
		state->initResearchTechs();
		
		return std::move(state);
	}
	
	size_t GameConfig::getNumberOfPlayers() const
	{
		return numPlayers == -1 ? agentParams.size() : numPlayers;
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