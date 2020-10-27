#include <filesystem>
#include <GameStateRenderer.h>
#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>
#include <Game/TBSGameCommunicator.h>
#include <yaml-cpp/node/parse.h>

int main()
{
	std::mt19937 rngEngine(0ll);

	// Read Config
	std::filesystem::path configPath("../../../gameConfigs/KillTheKing.yaml");
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto renderConfig = yamlConfig.as<SGA::RenderConfig>();
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	// Initialize the game
	auto game = SGA::generateGameFromConfig(gameConfig, rngEngine);
	
	int playerID = 0;
	int humanPlayerID=-1;
	auto agents = SGA::agentsFromConfig(gameConfig);
	
	const std::uniform_int_distribution<unsigned int> distribution(0,std::numeric_limits<unsigned int>::max());
	for(size_t i = 0; i < gameConfig.getNumberOfPlayers(); i++)
	{
		auto agent = std::move(agents[i]);
		// This is a human player, treat is as an non existing agent. The Renderer will handle it
		if (agent == nullptr)
		{
			humanPlayerID = playerID;
			playerID++;
			continue;
		}
		
		if(gameConfig.gameType == "TBS")
		{
			std::unique_ptr<SGA::TBSGameCommunicator> comm = std::make_unique<SGA::TBSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(comm));
		}
		else
		{
			std::unique_ptr<SGA::RTSGameCommunicator> comm = std::make_unique<SGA::RTSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::RTSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(comm));			
		}
		
		playerID++;
	}
	
	//Build Navmesh
	if (gameConfig.gameType == "RTS")
	{
		auto& gameRTS = dynamic_cast<SGA::RTSGame&>(*game);
		const SGA::RTSForwardModel& fm = gameRTS.getForwardModel();
		SGA::RTSGameState& state = *gameRTS.gameState;
		fm.buildNavMesh(state);
	}
	


	// Initialize the gameRenderer
	// We change the current_path to load sprites relative to the folder containing the configuration file
	auto tmp = std::filesystem::current_path();
	current_path(absolute(configPath.parent_path()));
	auto stateRenderer = SGA::stateRendererFromConfig(*game, renderConfig, gameConfig, humanPlayerID);
	current_path(tmp);

	game->addCommunicator(std::move(stateRenderer));
	
	// Run the game
	game->run();
	
    return 0;
}
