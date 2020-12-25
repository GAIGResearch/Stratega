#include <Configuration/GameConfig.h>
#include <Configuration/BoardGenerator.h>
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
		//TODO ADD FM AbstractGeneration
		// Generate game
		std::unique_ptr<Game> game;
		if (config.gameType == "TBS")
		{
			auto gameState = generateAbstractTBSStateFromConfig(config, rngEngine);
			/*auto fm = generateTBSforwardModelFromConfig(config);*/

			SGA::TBSForwardModel fm;
			//SGA::RTSForwardModel fm;
			gameState->turnLimit = 100000000000;
			fm.winCondition = SGA::WinConditionType::UnitAlive;
			fm.unitTypeID = 1;
			
			game = std::make_unique<TBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if (config.gameType == "RTS")
		{
			auto board = config.boardGenerator->generate(rngEngine);
			auto gameState = generateAbstractRTSStateFromConfig(config, rngEngine);
			SGA::RTSForwardModel fm;
			//SGA::RTSForwardModel fm;
			
			fm.winCondition = SGA::WinConditionType::UnitAlive;
			fm.unitTypeID = 1;
			game = std::make_unique<RTSGame>(std::move(gameState), fm, rngEngine);
		}
		else
		{
			throw std::runtime_error("Tried generating a game with unknown game-type " + config.gameType);
		}

		return game;
	}
	
	std::unique_ptr<RTSGameState> generateAbstractRTSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		// Initialize state
		auto board = config.boardGenerator->generate(rngEngine);
		auto state = std::make_unique<RTSGameState>(std::move(board), config.tileTypes);

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
	
	std::unique_ptr<TBSGameState> generateAbstractTBSStateFromConfig(const GameConfig& config, std::mt19937& rngEngine)
	{
		// Initialize state
		auto board = config.boardGenerator->generate(rngEngine);
		auto state = std::make_unique<TBSGameState>(std::move(board), config.tileTypes);
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

	throw std::runtime_error("Unknown action with name " + name);
}

int SGA::GameConfig::getTileID(const std::string& name) const
{
	for (const auto& idTypePair : tileTypes)
	{
		if (idTypePair.second.name == name)
			return idTypePair.first;
	}

	throw std::runtime_error("Unknown tile with name " + name);
}
