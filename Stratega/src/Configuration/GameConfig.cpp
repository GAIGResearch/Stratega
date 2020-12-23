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

	std::unique_ptr<Game> generateAbstractGameFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		//TODO ADD FM AbstractGeneration
		// Generate game
		std::unique_ptr<Game> game;
		if (config.gameType == "TBS")
		{
			auto gameState = generateAbstractTBSStateFromConfig(config, rngEngine);
			/*auto fm = generateTBSforwardModelFromConfig(config);*/

			SGA::TBSAbstractForwardModel fm;
			//SGA::RTSAbstractForwardModel fm;
			gameState->turnLimit = 100000000000;
			fm.winCondition = SGA::WinConditionType::UnitAlive;
			fm.unitTypeID = 1;
			
			game = std::make_unique<AbstractTBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if (config.gameType == "RTS")
		{
			auto generator = boardGeneratorFromConfig(config);
			auto board = generator->generate(rngEngine);
			auto gameState = generateAbstractRTSStateFromConfig(config, rngEngine);
			SGA::RTSAbstractForwardModel fm;
			//SGA::RTSAbstractForwardModel fm;
			
			fm.winCondition = SGA::WinConditionType::UnitAlive;
			fm.unitTypeID = 1;
			game = std::make_unique<AbstractRTSGame>(std::move(gameState), fm, rngEngine);
		}
		else
		{
			throw std::runtime_error("Tried generating a game with unknown game-type " + config.gameType);
		}

		return game;
	}
	
	std::unique_ptr<RTSGameState2> generateAbstractRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		auto boardGenerator = boardGeneratorFromConfig(config);
		auto tileTypes = tileTypesFromConfig(config);

		// Convert the unordered maps
		std::unordered_map<int, TileType> tileTypesMap;
		for (const auto& nameTypePair : tileTypes)
		{
			tileTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}

		// Initialize state
		auto board = boardGenerator->generate(rngEngine);
		auto state = std::make_unique<RTSGameState2>(std::move(board), std::move(tileTypesMap));

		state->entityTypes = std::make_shared<std::unordered_map<int, EntityType>>(config.entityTypes);
		state->entityGroups = config.entityGroups;
		//std::unordered_map<int, ActionType> copy = std::move(config.actionTypes);
		state->actionTypes = std::make_shared<std::unordered_map<int, ActionType>>(config.actionTypes);
		state->parameterIDLookup = std::make_shared<std::unordered_map<std::string, ParameterID>>(config.parameters);
		std::vector<int> playerIDs;
		for (auto i = 0; i < config.getNumberOfPlayers(); i++)
		{
			playerIDs.push_back(state->addPlayer());
		}




		//Add entity
		SGA::Entity building1;
		building1.id = 3;
		building1.owner = 0;
		building1.actionTypeIds.emplace_back(3);
		building1.position = SGA::Vector2f(4, 11);
		building1.typeID = 1;
		building1.parameters.emplace_back(200);
		building1.parameters.emplace_back(10);

		state->entities.emplace_back(building1);
		
		SGA::Entity building2;
		building2.id = 2;
		building2.owner = 1;
		building2.actionTypeIds.emplace_back(3);
		building2.position = SGA::Vector2f(6, 12);
		building2.typeID = 1;
		building2.parameters.emplace_back(200);
		building2.parameters.emplace_back(10);

		state->entities.emplace_back(building2);

		//Add entity
		SGA::Entity entity;
		entity.id = 0;
		entity.owner = 0;
		//entity.actionTypeIds.emplace_back(1);
		entity.actionTypeIds.emplace_back(2);
		entity.position = SGA::Vector2f(5, 3);
		entity.typeID = 0;
		entity.parameters.emplace_back(60);
		entity.parameters.emplace_back(0);

		state->entities.emplace_back(entity);

		SGA::Entity entity2;
		entity2.id = 1;
		entity2.owner = 1;
		//entity2.actionTypeIds.emplace_back(1);
		entity2.actionTypeIds.emplace_back(2);
		entity2.position = SGA::Vector2f(3, 3);
		entity2.typeID = 0;
		entity2.parameters.emplace_back(60);
		entity2.parameters.emplace_back(0);

		state->entities.emplace_back(entity2);

		
		//// Spawn units
		//// TODO Unit spawn configuration
		//std::unordered_set<Vector2i> occupiedSet;
		//std::uniform_int_distribution<int> xDist(0, state->board.getWidth() - 1);
		//std::uniform_int_distribution<int> yDist(0, state->board.getHeight() - 1);
		//for (auto i = 0; i < state->players.size(); i++)
		//{
		//	for (const auto& nameTypePair : unitTypes)
		//	{
		//		// Generate random positions until a suitable was found
		//		Vector2i pos(xDist(rngEngine), yDist(rngEngine));
		//		while (!state->board.getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
		//		{
		//			pos.x = xDist(rngEngine);
		//			pos.y = yDist(rngEngine);
		//		}
		//		occupiedSet.insert(pos);

		//		state->addUnit(playerIDs[i], nameTypePair.second.id, pos);
		//	}
		//}

		return std::move(state);
	}
	
	std::unique_ptr<TBSGameState2> generateAbstractTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		auto boardGenerator = boardGeneratorFromConfig(config);
		auto tileTypes = tileTypesFromConfig(config);
		
		// Convert the unordered maps
		std::unordered_map<int, TileType> tileTypesMap;
		for (const auto& nameTypePair : tileTypes)
		{
			tileTypesMap.emplace(nameTypePair.second.id, nameTypePair.second);
		}

		// Initialize state
		auto board = boardGenerator->generate(rngEngine);
		auto state = std::make_unique<TBSGameState2>(std::move(board), std::move(tileTypesMap));
		state->entityTypes = std::make_shared<std::unordered_map<int, EntityType>>(config.entityTypes);
		state->entityGroups = config.entityGroups;
		state->actionTypes = std::make_shared<std::unordered_map<int, ActionType>>(config.actionTypes);
		state->parameterIDLookup = std::make_shared<std::unordered_map<std::string, ParameterID>>(config.parameters);
		
		state->turnLimit = config.roundLimit;
		std::vector<int> playerIDs;
		for (auto i = 0; i < config.getNumberOfPlayers(); i++)
		{
			playerIDs.push_back(state->addPlayer());
		}





		//Add entity
		SGA::Entity building1;
		building1.id = 3;
		building1.owner = 0;
		building1.actionTypeIds.emplace_back(3);
		building1.position = SGA::Vector2f(3, 11);
		building1.typeID = 1;
		building1.parameters.emplace_back(200);
		building1.parameters.emplace_back(10);

		state->entities.emplace_back(building1);

		SGA::Entity building2;
		building2.id = 2;
		building2.owner = 1;
		building2.actionTypeIds.emplace_back(3);
		building2.position = SGA::Vector2f(6, 12);
		building2.typeID = 1;
		building2.parameters.emplace_back(200);
		building2.parameters.emplace_back(10);

		state->entities.emplace_back(building2);

		//Add entity
		SGA::Entity entity;
		entity.id = 0;
		entity.owner = 0;
		//entity.actionTypeIds.emplace_back(1);
		entity.actionTypeIds.emplace_back(2);
		entity.position = SGA::Vector2f(5, 3);
		entity.typeID = 0;
		entity.parameters.emplace_back(60);
		entity.parameters.emplace_back(0);

		state->entities.emplace_back(entity);

		SGA::Entity entity2;
		entity2.id = 1;
		entity2.owner = 1;
		//entity2.actionTypeIds.emplace_back(1);
		entity2.actionTypeIds.emplace_back(2);
		entity2.position = SGA::Vector2f(3, 3);
		entity2.typeID = 0;
		entity2.parameters.emplace_back(60);
		entity2.parameters.emplace_back(0);

		state->entities.emplace_back(entity2);

		
		// Spawn units
		// TODO Unit spawn configuration
		//std::unordered_set<Vector2i> occupiedSet;
		//std::uniform_int_distribution<int> xDist(0, state->board.getWidth() - 1);
		//std::uniform_int_distribution<int> yDist(0, state->board.getHeight() - 1);
		//for (auto i = 0; i < state->players.size(); i++)
		//{
		//	for (const auto& nameTypePair : unitTypes)
		//	{
		//		// Generate random positions until a suitable was found
		//		Vector2i pos(xDist(rngEngine), yDist(rngEngine));
		//		while (!state->board.getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
		//		{
		//			pos.x = xDist(rngEngine);
		//			pos.y = yDist(rngEngine);
		//		}
		//		occupiedSet.insert(pos);

		//		state->addUnit(playerIDs[i], nameTypePair.second.id, pos);
		//	}
		//}

		return std::move(state);
	}

}

int SGA::GameConfig::getEntityID(const std::string& name) const
{
	for (const auto& idTypePair : entityTypes)
	{
		if (idTypePair.second.name == name)
			return idTypePair.first;
	}

	throw std::runtime_error("Unknown entity with name " + name);
}

int SGA::GameConfig::getActionID(const std::string& name) const
{
	for (const auto& idTypePair : actionTypes)
	{
		if (idTypePair.second.name == name)
			return idTypePair.first;
	}

	throw std::runtime_error("Unknown entity with name " + name);
}
