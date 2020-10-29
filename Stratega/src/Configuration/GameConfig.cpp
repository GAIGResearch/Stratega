#include <Configuration/GameConfig.h>
#include <Configuration/BoardGenerator.h>
#include <Agent/AgentFactory.h>

namespace SGA
{
	std::vector<std::unique_ptr<Agent>> agentsFromConfig(const GameConfig& config)
	{
		std::vector<std::unique_ptr<Agent>> agents;
		for(const auto& nameParamPair : config.agentParams)
		{
			std::unique_ptr<Agent> agent;
			if(nameParamPair.second.IsNull())
			{
				agent = AgentFactory::get().createAgent(nameParamPair.first);
			}
			else
			{
				agent = AgentFactory::get().createAgent(nameParamPair.first, nameParamPair.second);
			}
		
			if(agent == nullptr && nameParamPair.first != "HumanAgent")
			{
				throw std::runtime_error("Unknown agent with name " + nameParamPair.first);
			}
		
			agents.emplace_back(std::move(agent));
		}

		return agents;
	}
	
	std::unordered_map<std::string, UnitType> unitTypesFromConfig(const GameConfig& config)
	{
		std::unordered_map<std::string, UnitType> unitLookup;
		int nextID = 0;
		for (const auto& nameConfigPair : config.unitTypes)
		{
			unitLookup.emplace(nameConfigPair.first, unitTypeFromConfig(nameConfigPair.second, nextID));
			nextID++;
		}

		return unitLookup;
	}
	
	std::unordered_map<std::string, TileType> tileTypesFromConfig(const GameConfig& config)
	{
		std::unordered_map<std::string, TileType> tileLookup;
		int nextID = 0;
		for(const auto& nameConfigPair : config.tileTypes)
		{
			tileLookup.emplace(nameConfigPair.first, tileTypeFromConfig(nameConfigPair.second, nextID));
			nextID++;
		}

		return tileLookup;
	}

	TBSForwardModel generateTBSforwardModelFromConfig(const GameConfig& config)
	{
		auto fmConfig = config.forwardModelConfig;
		TBSForwardModel fm;
		fm.winCondition = fmConfig.WinCondition;

		//Get unityTypeID for UnitAlive
		auto units = unitTypesFromConfig(config);		
		fm.unitTypeID = units[fmConfig.unitType].id;


		// Add effects
		auto tiles = tileTypesFromConfig(config);
		
		for(auto& effectConfig : fmConfig.effects)
		{
			Effect effect{};
			effect.type = effectConfig.second.type;
			effect.conditionType = effectConfig.second.condition;
			if(effect.type == EffectType::Death)
			{
				effect.targetTileTypeID = tiles.at(effectConfig.second.targetTileName).id;
			}
			else if(effect.type == EffectType::Damage)
			{
				effect.damage = effectConfig.second.amount;
			}

			if(effectConfig.second.trigger == TriggerType::EndOfTurn)
			{
				fm.addUnitEndOfTurnEffect(std::move(effect));
			}
			else if(effectConfig.second.trigger == TriggerType::EnterTile)
			{
				fm.addOnTileEnterEffect(std::move(effect));
			}
		}

		return fm;
	}

	RTSForwardModel generateRTSforwardModelFromConfig(const GameConfig& config)
	{
		auto fmConfig = config.forwardModelConfig;
		RTSForwardModel fm;
		fm.winCondition = fmConfig.WinCondition;

		//Get unityTypeID for UnitAlive
		auto units = unitTypesFromConfig(config);
		fm.unitTypeID = units[fmConfig.unitType].id;
		
		return fm;
	}
	
	std::unique_ptr<IBoardGenerator> boardGeneratorFromConfig(const GameConfig& config)
	{
		const auto& boardConfig = config.boardConfig;
		if (boardConfig.generationType == BoardGenerationType::Manual)
		{
			// Extract rows
			std::vector<std::string> rows;
			size_t last = 0, next;
			while ((next = boardConfig.layout.find(' ', last)) != std::string::npos)
			{
				rows.emplace_back(boardConfig.layout.substr(last, next - last));
				last = next + 1;
			}
			rows.push_back(boardConfig.layout.substr(last, boardConfig.layout.size() - last));

			// Initialize the generator
			std::unique_ptr<BoardGenerator> generator = std::make_unique<BoardGenerator>();
			generator->setDefaultRowPatterns(std::move(rows));
			auto tiles = tileTypesFromConfig(config);
			for (const auto& pair : tiles)
			{
				const auto& tileConfig = config.tileTypes.at(pair.first);
				generator->addTile(tileConfig.symbol, pair.second);
			}

			return std::move(generator);
		}

		throw std::runtime_error("Tried initiating a unknown board generation type");
	}

	std::unique_ptr<TBSGameState> generateTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		auto boardGenerator = boardGeneratorFromConfig(config);
		auto unitTypes = unitTypesFromConfig(config);
		auto tileTypes = tileTypesFromConfig(config);
		// Convert the unordered maps
		std::unordered_map<int, UnitType> unitTypesMap;
		std::unordered_map<int, TileType> tileTypesMap;
		for(const auto& nameTypePair : unitTypes)
		{
			unitTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}
		for (const auto& nameTypePair : tileTypes)
		{
			tileTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}

		// Initialize state
		auto board = boardGenerator->generate(rngEngine);
		auto state = std::make_unique<TBSGameState>(std::move(board), std::move(unitTypesMap), std::move(tileTypesMap));
		state->roundLimit = config.roundLimit;
		std::vector<int> playerIDs;
		for (auto i = 0; i < config.getNumberOfPlayers(); i++)
		{
			playerIDs.push_back(state->addPlayer());
		}

		// Spawn units
		// TODO Unit spawn configuration
		std::unordered_set<Vector2i> occupiedSet;
		std::uniform_int_distribution<int> xDist(0, state->getBoard().getWidth() - 1);
		std::uniform_int_distribution<int> yDist(0, state->getBoard().getHeight() - 1);
		for (auto i = 0; i < state->getPlayers().size(); i++)
		{
			for (const auto& nameTypePair : unitTypes)
			{
				// Generate random positions until a suitable was found
				Vector2i pos(xDist(rngEngine), yDist(rngEngine));
				while (!state->getBoard().getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
				{
					pos.x = xDist(rngEngine);
					pos.y = yDist(rngEngine);
				}
				occupiedSet.insert(pos);

				state->addUnit(playerIDs[i], nameTypePair.second.id, pos);
			}
		}

		return std::move(state);
	}

	std::unique_ptr<RTSGameState> generateRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		auto boardGenerator = boardGeneratorFromConfig(config);
		auto unitTypes = unitTypesFromConfig(config);
		auto tileTypes = tileTypesFromConfig(config);
		// Convert the unordered maps
		std::unordered_map<int, UnitType> unitTypesMap;
		std::unordered_map<int, TileType> tileTypesMap;
		for (const auto& nameTypePair : unitTypes)
		{
			unitTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}
		for (const auto& nameTypePair : tileTypes)
		{
			tileTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}

		// Initialize state
		auto board = boardGenerator->generate(rngEngine);
		auto state = std::make_unique<RTSGameState>(std::move(board), std::move(unitTypesMap), std::move(tileTypesMap));
		
		std::vector<int> playerIDs;
		for (auto i = 0; i < config.getNumberOfPlayers(); i++)
		{
			playerIDs.push_back(state->addPlayer());
		}

		// Spawn units
		// TODO Unit spawn configuration
		std::unordered_set<Vector2i> occupiedSet;
		std::uniform_int_distribution<int> xDist(0, state->getBoard().getWidth() - 1);
		std::uniform_int_distribution<int> yDist(0, state->getBoard().getHeight() - 1);
		for (auto i = 0; i < state->players.size(); i++)
		{
			for (const auto& nameTypePair : unitTypes)
			{
				// Generate random positions until a suitable was found
				Vector2i pos(xDist(rngEngine), yDist(rngEngine));
				while (!state->getBoard().getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
				{
					pos.x = xDist(rngEngine);
					pos.y = yDist(rngEngine);
				}
				occupiedSet.insert(pos);

				state->addUnit(playerIDs[i], nameTypePair.second.id, pos);
			}
		}

		return std::move(state);
	}

	std::unique_ptr<Game> generateGameFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		// Generate game
		std::unique_ptr<Game> game;
		if(config.gameType == "TBS")
		{
			auto gameState = generateTBSStateFromConfig(config, rngEngine);
			auto fm = generateTBSforwardModelFromConfig(config);
			game = std::make_unique<TBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if(config.gameType == "RTS")
		{
			auto generator = boardGeneratorFromConfig(config);
			auto board = generator->generate(rngEngine);
			auto gameState = generateRTSStateFromConfig(config, rngEngine);
			auto fm = generateRTSforwardModelFromConfig(config);
			game = std::make_unique<RTSGame>(std::move(gameState), fm, rngEngine);
		}
		else
		{
			throw std::runtime_error("Tried generating a game with unknown game-type " + config.gameType);
		}

		return game;
	}
}
