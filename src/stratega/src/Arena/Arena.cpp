#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Logging/Log.h>
#include <Stratega/Agent/Agent.h>

#include <Stratega/Arena/Arena.h>
#include <Stratega/Arena/utils.h>

Arena::Arena(const SGA::GameConfig& newConfig)
	: config(&newConfig), runner(createGameRunner(newConfig)), gameBattleCount(0)
{
}

void Arena::runGames(int playerCount, int seed, int gamesNumber, int mapNumber)
{
	currentMapID = 0;
	currentSeed = seed;
	
	try
	{
		for (int i = 0; i < gamesNumber * mapNumber; ++i)
		{
			gameCount = i;
			gameBattleCount = 0;

			//Change map after playing all gamesNumber in each map
			if (i % gamesNumber == 0 && i != 0)
			{
				currentMapID++;
			}
			currentSeed = seed + i;
			boost::mt19937 rngEngine(static_cast<unsigned>(currentSeed));
			//std::cout << "Using Seed: " << currentSeed << std::endl;
            SGA::LoggingScope scope("Game " + std::to_string(gameCount));
            SGA::logSingleValue("Map", std::to_string(currentMapID));
            SGA::logSingleValue("Seed", std::to_string(currentSeed));
			CallbackFn callback = [&](const std::vector<int>& c) {runGame(c, rngEngine); };
			generateCombinations(config->agentParams.size(), static_cast<size_t>(playerCount), callback);
            SGA::getDefaultLogger().flush();
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Arena error: " << ex.what()<< std::endl;		
	}	
}

void Arena::runGames(int playerCount, int seed, int gamesNumber, int mapNumber, std::vector<std::shared_ptr<SGA::Agent>> agents)
{
	currentMapID = 0;
	currentSeed = seed;

	try
	{
		for (int i = 0; i < gamesNumber * mapNumber; ++i)
		{
			gameCount = i;
			gameBattleCount = 0;

			//Change map after playing all gamesNumber in each map
			if (i % gamesNumber == 0 && i != 0)
			{
				currentMapID++;
			}
			currentSeed = seed + i;
			boost::mt19937 rngEngine(static_cast<unsigned>(currentSeed));
			std::cout << "Using Seed: " << currentSeed << std::endl;
			CallbackFn callback = [&](const std::vector<int>& c) {runGame(c, rngEngine,agents); };
			generateCombinations(config->agentParams.size(), static_cast<size_t>(playerCount), callback);
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Arena error: " << ex.what() << std::endl;
	}
}

void Arena::runGame(const std::vector<int>& agentAssignment, boost::mt19937 rngEngine)
{
	// Initialize new Game
	std::cout << "Initializing new game" << std::endl;
	runner->reset(currentMapID);

	// Assign agents
	boost::random::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
	auto allAgents = config->generateAgents();
	std::vector<std::shared_ptr<SGA::Agent>> agents(agentAssignment.size());
	boost::random::uniform_int_distribution<unsigned int> seedDist(0, boost::mt19937::max());
	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		agents[i] = std::move(allAgents[static_cast<size_t>(agentAssignment[i])]);

		//Check if agent is Human
		if (!agents[i])
			throw std::runtime_error("Human agents cant play Arena");

		std::cout << "Player " << i << " is controlled by " << agents[i]->getName() << std::endl;

		// Set seed of the agents for deterministic behaviour
        unsigned int seed_one = seedDist(rngEngine);
        agents[i]->setSeed(seed_one);
	}

	// Initialize logging
    SGA::LoggingScope battleScope("Battle" + std::to_string(gameBattleCount++));

	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		//SGA::logValue("PlayerAssignment", config->agentParams[static_cast<size_t>(agentAssignment[i])].first);
        SGA::logValue("PlayerAssignment", agents[i]->getName());
	}
	
	// Run the game
	try
	{
		runner->run(agents.begin(), agents.end(), this);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Game crashed error, logging error in the logfile" << std::endl;
		SGA::logValue("Error", std::string(ex.what()));
	}
	std::cout << std::endl;

	// Kinda dirty hack to flush after every game
	// This could result in weird yaml files, if logging is done outside of the game loggingScope
	//SGA::getDefaultLogger().flush();
}

void Arena::runGame(const std::vector<int>& agentAssignment, boost::mt19937 rngEngine, std::vector<std::shared_ptr<SGA::Agent>> newAgents)
{
	// Initialize new Game
	std::cout << "Initializing new game" << std::endl;
	runner->reset(currentMapID);

	// Assign agents
	boost::random::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
	auto allAgents = newAgents;
	std::vector<std::shared_ptr<SGA::Agent>> agents(agentAssignment.size());
	boost::random::uniform_int_distribution<unsigned int> seedDist(0, boost::mt19937::max());
	for (size_t i = 0; i < agentAssignment.size(); i++)
	{
		agents[i] = std::move(allAgents[static_cast<size_t>(agentAssignment[i])]);

		//Check if agent is Human
		if (!agents[i])
			throw std::runtime_error("Human agents cant play Arena");

		std::cout << "Player " << i << " is controlled by " << agents[i]->getName() << std::endl;

		// Set seed of the agents for deterministic behaviour
		agents[i]->setSeed(seedDist(rngEngine));
	}

	// Initialize logging	
	SGA::LoggingScope scope("Game " + std::to_string(gameCount));
	SGA::logSingleValue("Map", std::to_string(currentMapID));
	SGA::logSingleValue("Battle", std::to_string(gameBattleCount++));
	SGA::logSingleValue("Seed", std::to_string(currentSeed));
	for (size_t i = 0; i < agentAssignment.size(); i++)
	{
		SGA::logValue("PlayerAssignment", config->agentParams[static_cast<size_t>(agentAssignment[i])].first);
	}

	// Run the game
	try
	{
		runner->run(agents.begin(), agents.end(), this);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Game crashed error, logging error in the logfile" << std::endl;
		SGA::logValue("Error", std::string(ex.what()));
	}
	std::cout << std::endl;

	// Kinda dirty hack to flush after every game
	// This could result in weird yaml files, if logging is done outside of the game loggingScope
	SGA::getDefaultLogger().flush();
}

void Arena::onGameStateAdvanced(const SGA::GameState& state, const SGA::ForwardModel& forwardModel)
{
	if(state.getGameType() == SGA::GameType::TBS)
	{
		SGA::logValue("ActivePlayer", state.getCurrentTBSPlayer());

		// ToDo getActions should accept const gameStates
		auto actions = forwardModel.generateActions(state, state.getCurrentTBSPlayer());
		SGA::logValue("ActionCount", actions.size());
	}
	else if(state.getGameType() == SGA::GameType::RTS)
	{
		for (size_t i = 0; i < static_cast<size_t>(state.getNumPlayers()); i++)
		{
			int playerID = state.getPlayers()[i].getID();
			SGA::LoggingScope playerScope("Player" + std::to_string(playerID));
			auto actions = forwardModel.generateActions(state, playerID);
			SGA::logValue("ActionCount", actions.size());
		}
	}
}

void Arena::onGameFinished(const SGA::GameState& finalState, const SGA::ForwardModel& /*forwardModel*/)
{
	if (finalState.getGameType() == SGA::GameType::TBS)
	{
		SGA::logSingleValue("WinnerID", finalState.getWinnerID());
		SGA::logSingleValue("Turns", finalState.getCurrentTick() + 1);

		std::cout<<"Winner is: "<< finalState.getWinnerID() << "\n";
	}
	else if (finalState.getGameType() == SGA::GameType::RTS)
	{
		SGA::logSingleValue("WinnerID", finalState.getWinnerID());
	}
}

