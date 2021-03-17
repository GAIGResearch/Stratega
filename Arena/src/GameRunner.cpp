#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Logging/Log.h>

#include <GameRunner.h>
#include <utils.h>
#include <RTSLogger.h>
#include <TBSLogger.h>

GameRunner::GameRunner(const SGA::GameConfig& config)
	: config(&config), gameCount(0)
{
}

void GameRunner::runGames(int playerCount, int seed)
{
	gameCount = 0;
	std::mt19937 rngEngine(seed);
	CallbackFn callback = [&](const std::vector<int>& c) {runGame(c, rngEngine);};
	generateCombinations(config->agentParams.size(), playerCount, callback);
}

void GameRunner::runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine)
{
	std::cout << "Initializing new game" << std::endl;
	auto game = generateAbstractGameFromConfig(*config, rngEngine);
	std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
	auto agents = config->generateAgents();
	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		std::cout << "Player " << i << " is controlled by " << config->agentParams[agentAssignment[i]].first << std::endl;

		auto agent = std::move(agents[agentAssignment[i]]);
		if(agent == nullptr)
		{
			throw std::runtime_error("Human-agents are not allowed in arena mode.");
		}
		
		if (config->gameType == SGA::GameType::TBS)
		{
			auto comm = std::make_unique<SGA::AgentGameCommunicator>(i);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(comm));
			
		}
		else
		{
			auto comm = std::make_unique<SGA::RTSGameCommunicator>(i);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::RTSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(comm));
		}		
	}

	//Build Navmesh
	if (config->gameType == SGA::GameType::RTS)
	{
		auto& gameRTS = dynamic_cast<SGA::RTSGame&>(*game);
		const SGA::RTSForwardModel& fm = gameRTS.getForwardModel();
		SGA::GameState state = gameRTS.getStateCopy();
		fm.buildNavMesh(state, SGA::NavigationConfig());
	}

	// Add logger
	if(config->gameType == SGA::GameType::TBS)
	{
		game->addCommunicator(std::make_unique<TBSLogger>(dynamic_cast<SGA::TBSGame&>(*game)));
	}
	else
	{
		game->addCommunicator(std::make_unique<RTSLogger>(dynamic_cast<SGA::RTSGame&>(*game)));
	}

	gameCount++;
	SGA::LoggingScope scope("Game" + std::to_string(gameCount));
	for(size_t i = 0; i < agentAssignment.size(); i++)
	{
		SGA::Log::logValue("PlayerAssignment", config->agentParams[agentAssignment[i]].first);
	}

	// Run the game
	try
	{
		game->run();
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