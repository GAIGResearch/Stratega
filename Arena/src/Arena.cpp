#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Logging/Log.h>
#include <Stratega/Agent/Agent.h>

#include <Arena.h>
#include <utils.h>

Arena::Arena(const SGA::GameConfig& config)
	: config(&config), runner(createGameRunner(config)), gameBattleCount(0)
{
}

void Arena::runGames(int playerCount, int seed, int gamesNumber, int mapNumber)
{
	currentMapID = 0;
	currentSeed = seed;
	
	for (int i = 0; i < gamesNumber*mapNumber; ++i)
	{
		gameCount = i;
		gameBattleCount = 0;
		
		//Change map after playing all gamesNumber in each map
		if(i% gamesNumber ==0&&i!=0)
		{
			currentMapID++;
		}
		currentSeed =seed+i;
		std::mt19937 rngEngine(currentSeed);
		std::cout << "Using Seed: " << currentSeed <<std::endl;
		CallbackFn callback = [&](const std::vector<int>& c) {runGame(c, rngEngine); };
		generateCombinations(config->agentParams.size(), playerCount, callback);
	}
	
}

void Arena::runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine)
{
	// Initialize new Game
	std::cout << "Initializing new game" << std::endl;
	runner->reset(currentMapID);

	// Assign agents
	std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
	auto allAgents = config->generateAgents();
	std::vector<std::unique_ptr<SGA::Agent>> agents(agentAssignment.size());
	std::uniform_int_distribution<unsigned int> seedDist(0, std::mt19937::max());
	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		std::cout << "Player " << i << " is controlled by " << config->agentParams[agentAssignment[i]].first << std::endl;
		agents[i] = std::move(allAgents[agentAssignment[i]]);

		//Check if agent is Human
		if (!agents[i])
			throw std::runtime_error("Human agents cant play Arena");

		// Set seed of the agents for deterministic behaviour
		agents[i]->setSeed(seedDist(rngEngine));
	}

	// Initialize logging	
	SGA::LoggingScope scope("Game " + std::to_string(gameCount));
	SGA::Log::logSingleValue("Map", std::to_string(currentMapID));
	SGA::Log::logSingleValue("Battle", std::to_string(gameBattleCount++));
	SGA::Log::logSingleValue("Seed", std::to_string(currentSeed));
	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		SGA::Log::logValue("PlayerAssignment", config->agentParams[agentAssignment[i]].first);
	}
	
	// Run the game
	try
	{
		runner->run(agents, this);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Game crashed error logging error in the logfile" << std::endl;
		SGA::Log::logValue("Error", std::string(ex.what()));
	}
	std::cout << std::endl;

	// Kinda dirty hack to flush after every game
	// This could result in weird yaml files, if logging is done outside of the game loggingScope
	SGA::Log::getDefaultLogger().flush();
}

void Arena::onGameStateAdvanced(const SGA::GameState& state, const SGA::EntityForwardModel& forwardModel)
{
	if(state.gameType == SGA::GameType::TBS)
	{
		SGA::Log::logValue("ActivePlayer", state.currentPlayer);

		// ToDo getActions should accept const gameStates
		auto actions = forwardModel.generateActions(state, state.currentPlayer);
		SGA::Log::logValue("ActionCount", actions.size());
	}
	else if(state.gameType == SGA::GameType::RTS)
	{
		for (size_t i = 0; i < state.players.size(); i++)
		{
			int playerID = state.players[i].id;
			SGA::LoggingScope playerScope("Player" + std::to_string(playerID));
			auto actions = forwardModel.generateActions(state, playerID);
			SGA::Log::logValue("ActionCount", actions.size());
		}
	}
}

void Arena::onGameFinished(const SGA::GameState& finalState, const SGA::EntityForwardModel& forwardModel)
{
	if (finalState.gameType == SGA::GameType::TBS)
	{
		SGA::Log::logSingleValue("WinnerID", finalState.winnerPlayerID);
		SGA::Log::logSingleValue("Turns", finalState.currentTick + 1);
	}
	else if (finalState.gameType == SGA::GameType::RTS)
	{
		SGA::Log::logSingleValue("WinnerID", finalState.winnerPlayerID);
	}
}

