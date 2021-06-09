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

	int GameConfig::addPlayer(std::unique_ptr<GameState>& state, GameInfo gameInfo) const
	{
		int nextPlayerID = state->getNumPlayers();
		Player player = { nextPlayerID, 0, true };

		
		// Add parameters
		player.parameters.resize(gameInfo.playerParameterTypes->size());
		for (const auto& idParamPair : *gameInfo.playerParameterTypes)
		{
			player.parameters[idParamPair.second.index] = idParamPair.second.defaultValue;
		}


		// Add actions
		player.attachedActions.reserve(playerActionIds.size());
		for (auto actionTypeID : playerActionIds)
		{
			player.attachedActions.emplace_back(ActionInfo{ actionTypeID, 0 });
		}

		state->addPlayer(player);
		return player.id;
	}

	std::unique_ptr<GameState> GameConfig::generateGameState(int levelID) const
	{
		// Initialize state
		std::unique_ptr<GameState> state = std::make_unique<GameState>();
		state->gameType = gameType;
		state->tickLimit = tickLimit;

		//GameInfo
		GameInfo gameInfo;		
		gameInfo.entityTypes = std::make_shared<std::unordered_map<int, EntityType>>(entityTypes);
		gameInfo.tileTypes = std::make_shared<std::unordered_map<int, TileType>>(tileTypes);
		gameInfo.playerParameterTypes = std::make_shared<std::unordered_map<ParameterID, Parameter>>(playerParameterTypes);
		gameInfo.entityGroups = entityGroups;
		gameInfo.actionTypes = std::make_shared<std::unordered_map<int, ActionType>>(actionTypes);
		gameInfo.parameterIDLookup = std::make_shared<std::unordered_map<std::string, ParameterID>>(parameters);
		gameInfo.technologyTreeCollection = std::make_shared<TechnologyTreeCollection>(technologyTreeCollection);
		gameInfo.playerSpawnableTypes = std::make_shared<std::unordered_set<EntityTypeID>>(playerSpawnableTypes);
		gameInfo.yamlPath = yamlPath;
		gameInfo.gameDescription = std::make_shared<GameDescription>(actionCategories, entityCategories);
		state->gameInfo = std::make_shared<GameInfo>(gameInfo);

		
		std::unordered_set<int> playerIDs;
		for (auto i = 0; i < getNumberOfPlayers(); i++)
		{
			//playerIDs.emplace(state->addPlayer(playerActionIds));
			playerIDs.emplace(addPlayer(state, gameInfo));
		}

		// Create some lookups for initializing the board and entities
		std::unordered_map<char, const TileType*> tileLookup;
		const auto* defaultTile = &state->gameInfo->tileTypes->begin()->second;
		for(const auto& idTilePair : *state->gameInfo->tileTypes)
		{
			tileLookup.emplace(idTilePair.second.symbol, &idTilePair.second);
			if (idTilePair.second.isDefaultTile)
				defaultTile = &idTilePair.second;
		}

		std::unordered_map<char, const EntityType*> entityLookup;
		for(const auto& idEntityPair : *state->gameInfo->entityTypes)
		{
			entityLookup.emplace(idEntityPair.second.symbol, &idEntityPair.second);
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
		
		//Assign board to state
		state->board = Grid2D<Tile>(board.getWidth(), tiles.begin(), tiles.end());

		// Initialize Pathfinding
		if(gameType == GameType::RTS)
		{
			auto* rtsFM = dynamic_cast<SGA::RTSForwardModel*>(forwardModel.get());
			rtsFM->buildNavMesh(*state, NavigationConfig{});
		}

		//Initialize researched list for each player
		for (int i = 0; i < getNumberOfPlayers(); i++)
		{
			state->researchedTechnologies[i] = {};
		}
		
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
			if (idTypePair.second.name == name)
				return idTypePair.first;
		}

		throw std::runtime_error("Unknown entity with name " + name);
	}

	int GameConfig::getActionID(const std::string& name) const
	{
		for (const auto& idTypePair : actionTypes)
		{
			if (idTypePair.second.name == name)
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
			if (idTypePair.second.name == name)
				return idTypePair.first;
		}

		throw std::runtime_error("Unknown tile with name " + name);
	}
}