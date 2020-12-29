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
			fm.targetUnitTypeID = 1;
			
			game = std::make_unique<TBSGame>(std::move(gameState), std::move(fm), rngEngine);
		}
		else if (config.gameType == "RTS")
		{
			auto board = config.boardGenerator->generate(rngEngine);
			auto gameState = generateAbstractRTSStateFromConfig(config, rngEngine);
			SGA::RTSForwardModel fm;
			//SGA::RTSForwardModel fm;
			
			fm.winCondition = SGA::WinConditionType::UnitAlive;
			fm.targetUnitTypeID = 1;
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

		// Spawn units
		// TODO Unit spawn configuration
		std::unordered_set<Vector2i> occupiedSet;
		std::uniform_int_distribution<int> xDist(0, state->board.getWidth() - 1);
		std::uniform_int_distribution<int> yDist(0, state->board.getHeight() - 1);
		for (const auto& player : state->players)
		{
			for (const auto& nameTypePair : config.entityTypes)
			{
				// Generate random positions until a suitable was found
				Vector2i pos(xDist(rngEngine), yDist(rngEngine));
				while (!state->board.getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
				{
					pos.x = xDist(rngEngine);
					pos.y = yDist(rngEngine);
				}
				occupiedSet.insert(pos);

				state->addEntity(nameTypePair.second, player.id, pos);
			}
		}

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

		// Spawn units
		// TODO Unit spawn configuration
		std::unordered_set<Vector2i> occupiedSet;
		std::uniform_int_distribution<int> xDist(0, state->board.getWidth() - 1);
		std::uniform_int_distribution<int> yDist(0, state->board.getHeight() - 1);
		for(const auto& player : state->players)
		{
			for (const auto& nameTypePair : config.entityTypes)
			{
				// Generate random positions until a suitable was found
				Vector2i pos(xDist(rngEngine), yDist(rngEngine));
				while (!state->board.getTile(pos.x, pos.y).isWalkable || occupiedSet.find(pos) != occupiedSet.end())
				{
					pos.x = xDist(rngEngine);
					pos.y = yDist(rngEngine);
				}
				occupiedSet.insert(pos);

				state->addEntity(nameTypePair.second, player.id, pos);
			}
		}

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
