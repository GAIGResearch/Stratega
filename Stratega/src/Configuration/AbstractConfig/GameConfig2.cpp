#include <Configuration/AbstractConfig/GameConfig2.h>
#include <Configuration/BoardGenerator.h>
#include <Agent/AgentFactory.h>

namespace SGA
{
	std::vector<std::unique_ptr<Agent>> GameConfig2::generateAgents()
	{
		std::vector<std::unique_ptr<Agent>> agents;
		for (const auto& nameParamPair : agentParams)
		{
			std::unique_ptr<Agent> agent;
			if (nameParamPair.second.IsNull())
			{
				agent = AgentFactory::get().createAgent(nameParamPair.first);
			}
			else
			{
				agent = AgentFactory::get().createAgent(nameParamPair.first, nameParamPair.second);
			}

			if (agent == nullptr && nameParamPair.first != "HumanAgent")
			{
				throw std::runtime_error("Unknown agent with name " + nameParamPair.first);
			}

			agents.emplace_back(std::move(agent));
		}

		return agents;
	}

	std::unordered_map<std::string, UnitType> GameConfig2::generateUnitTypes()
	{
		std::unordered_map<std::string, UnitType> unitLookup;
		int nextID = 0;
		for (const auto& nameConfigPair : unitTypes)
		{
			unitLookup.emplace(nameConfigPair.first, unitTypeFromConfig(nameConfigPair.second, nextID));
			nextID++;
		}

		return unitLookup;
	}

	std::unordered_map<std::string, TileType> GameConfig2::generateTileTypes()
	{
		std::unordered_map<std::string, TileType> tileLookup;
		int nextID = 0;
		for (const auto& nameConfigPair : tileTypes)
		{
			tileLookup.emplace(nameConfigPair.first, tileTypeFromConfig(nameConfigPair.second, nextID));
			nextID++;
		}

		return tileLookup;
	}

	TBSForwardModel GameConfig2::generateTBSforwardModel()
	{
		TBSForwardModel fm;
		fm.winCondition = forwardModelConfig.WinCondition;

		//Get unityTypeID for UnitAlive
		auto units = generateUnitTypes();
		fm.unitTypeID = units[forwardModelConfig.unitType].id;


		// Add effects
		auto tiles = generateTileTypes();
		for (auto& effectConfig : forwardModelConfig.effects)
		{
			EffectOld effect{};
			effect.type = effectConfig.second.type;
			effect.conditionType = effectConfig.second.condition;
			if (effect.type == EffectType::Death)
			{
				effect.targetTileTypeID = tiles.at(effectConfig.second.targetTileName).id;
			}
			else if (effect.type == EffectType::Damage)
			{
				effect.damage = effectConfig.second.amount;
			}

			if (effectConfig.second.trigger == TriggerType::EndOfTurn)
			{
				fm.addUnitEndOfTurnEffect(std::move(effect));
			}
			else if (effectConfig.second.trigger == TriggerType::EnterTile)
			{
				fm.addOnTileEnterEffect(std::move(effect));
			}
		}

		return fm;
	}

	RTSForwardModel GameConfig2::generateRTSforwardModel()
	{
		RTSForwardModel fm;
		fm.winCondition = forwardModelConfig.WinCondition;

		//Get unityTypeID for UnitAlive
		auto units = generateUnitTypes();
		fm.unitTypeID = units[forwardModelConfig.unitType].id;

		return fm;
	}

	std::unique_ptr<IBoardGenerator> GameConfig2::generateBoardGenerator()
	{
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
			auto tiles = generateTileTypes();
			for (const auto& pair : tiles)
			{
				const auto& tileConfig = tileTypes.at(pair.first);
				generator->addTile(tileConfig.symbol, pair.second);
			}

			return std::move(generator);
		}

		throw std::runtime_error("Tried initiating a unknown board generation type");
	}

	std::unique_ptr<TBSGameState> GameConfig2::generateTBSState(std::mt19937& rngEngine)
	{
		auto boardGenerator = generateBoardGenerator();
		auto unitTypes = generateUnitTypes();
		auto tileTypes = generateTileTypes();
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
		auto state = std::make_unique<TBSGameState>(std::move(board), std::move(unitTypesMap), std::move(tileTypesMap));
		state->roundLimit = roundLimit;
		std::vector<int> playerIDs;
		for (auto i = 0; i < getNumberOfPlayers(); i++)
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

	std::unique_ptr<RTSGameState> GameConfig2::generateRTSState(std::mt19937& rngEngine)
	{
		auto boardGenerator = generateBoardGenerator();
		auto unitTypes = generateUnitTypes();
		auto tileTypes = generateTileTypes();
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
		for (auto i = 0; i < getNumberOfPlayers(); i++)
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

	std::unique_ptr<Game> GameConfig2::generateGame(std::mt19937& rngEngine)
	{
		// Generate game
		std::unique_ptr<Game> game;
		if (gameType == "TBS")
		{
			auto gameState = generateTBSState(rngEngine);
			auto fm = generateTBSforwardModel();
			game = std::make_unique<TBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if (gameType == "RTS")
		{
			auto generator = generateBoardGenerator();
			auto board = generator->generate(rngEngine);
			auto gameState = generateRTSState(rngEngine);
			auto fm = generateRTSforwardModel();
			game = std::make_unique<RTSGame>(std::move(gameState), fm, rngEngine);
		}
		else
		{
			throw std::runtime_error("Tried generating a game with unknown game-type " + gameType);
		}

		return game;
	}
}
