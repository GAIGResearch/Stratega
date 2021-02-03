#include <Configuration/GameConfig.h>
#include <Agent/AgentFactory.h>

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
	
	std::unique_ptr<Game> generateAbstractGameFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		// Generate game
		std::unique_ptr<Game> game;
		if (config.gameType == ForwardModelType::TBS)
		{
			auto gameState = std::unique_ptr<TBSGameState>(dynamic_cast<TBSGameState*>(config.generateGameState().release()));
			auto fm = *dynamic_cast<TBSForwardModel*>(config.forwardModel.get());
			game = std::make_unique<TBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if (config.gameType == ForwardModelType::RTS)
		{
			auto gameState = std::unique_ptr<RTSGameState>(dynamic_cast<RTSGameState*>(config.generateGameState().release()));
			auto fm = *dynamic_cast<RTSForwardModel*>(config.forwardModel.get());
			game = std::make_unique<RTSGame>(std::move(gameState), fm, rngEngine);
		}
		else
		{
			throw std::runtime_error("Tried generating a game with unknown game-type ");
		}

		return game;
	}

	std::unique_ptr<GameState> GameConfig::generateGameState() const
	{
		// Initialize state
		std::unique_ptr<GameState> state;
		if(gameType == ForwardModelType::TBS)
		{
			state = std::make_unique<TBSGameState>();
		}
		else if(gameType == ForwardModelType::RTS)
		{
			state = std::make_unique<RTSGameState>();
		}

		// Assign data
		state->tickLimit = tickLimit;
		state->entityTypes = std::make_shared<std::unordered_map<int, EntityType>>(entityTypes);
		state->playerParameterTypes = std::make_shared<std::unordered_map<ParameterID, Parameter>>(playerParameterTypes);
		state->entityGroups = entityGroups;
		state->actionTypes = std::make_shared<std::unordered_map<int, ActionType>>(actionTypes);
		state->parameterIDLookup = std::make_shared<std::unordered_map<std::string, ParameterID>>(parameters);
		state->technologyTreeCollection = std::make_shared<TechnologyTreeCollection>(technologyTreeCollection);

		std::unordered_set<int> playerIDs;
		for (auto i = 0; i < getNumberOfPlayers(); i++)
		{
			playerIDs.emplace(state->addPlayer(playerActionIds));
		}

		// Create some lookups for initializing the board and entities
		std::unordered_map<char, const TileType*> tileLookup;
		const auto* defaultTile = &tileTypes.begin()->second;
		for(const auto& idTilePair : tileTypes)
		{
			tileLookup.emplace(idTilePair.second.symbol, &idTilePair.second);
			if (idTilePair.second.isDefaultTile)
				defaultTile = &idTilePair.second;
		}

		std::unordered_map<char, const EntityType*> entityLookup;
		for(const auto& idEntityPair : entityTypes)
		{
			entityLookup.emplace(idEntityPair.second.symbol, &idEntityPair.second);
		}

		// Configure board and spawn entities
		auto x = 0;
		auto y = 0;
		auto width = -1;
		std::vector<Tile> tiles;
		for(size_t i = 0; i < boardString.size(); i++)
		{
			auto c = boardString[i];
			if(c == '\n')
			{
				y++;
				if(width == -1)
				{
					width = x;
				}
				else if (x != width)
				{
					throw std::runtime_error("Line " + std::to_string(y) + " contains " + std::to_string(x) + " symbols. Expected " + std::to_string(width));
				}
				
				x = 0;
				continue;
			}

			auto entityIt = entityLookup.find(c);
			auto tileIt = tileLookup.find(c);
			if(entityIt != entityLookup.end())
			{
				// Check if the entity was assigned to an player, we only look for players with ID 0-9
				auto ownerID = Player::NEUTRAL_PLAYER_ID;
				if(i < boardString.size() - 1 && std::isdigit(boardString[i + 1]))
				{
					ownerID = static_cast<int>(boardString[i + 1] - '0'); // Convert char '0','1',... to the corresponding integer
					if(playerIDs.find(ownerID) == playerIDs.end())
					{
						throw std::runtime_error("Tried assigning the entity " + entityIt->second->name + " to an unknown player " + std::to_string(ownerID));
					}
					i++;
				}

				state->addEntity(*entityIt->second, ownerID, Vector2f(x, y));
				// Since an entity occupied this position, we will place the default tile here
				tiles.emplace_back(defaultTile->toTile(x, y));
			}
			else if(tileIt != tileLookup.end())
			{
				tiles.emplace_back(tileIt->second->toTile(x, y));
			}
			else
			{
				throw std::runtime_error("Encountered unknown symbol '" + std::string(1, c) + "'when parsing the board.");
			}

			x++;
		}

		// Sometimes there is a newLine at the end of the string, and sometimes not
		if(boardString[boardString.size() - 1] != '\n')
		{
			y++;
			if (x != width)
			{
				throw std::runtime_error("Line " + std::to_string(y) + " contains " + std::to_string(x) + " symbols. Expected " + std::to_string(width));
			}
		}
		
		state->board = Grid2D<Tile>(width, tiles.begin(), tiles.end());

		return std::move(state);
	}

	int GameConfig::getNumberOfPlayers() const
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