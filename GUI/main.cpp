#include <filesystem>
#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>
#include <Game/TBSGameCommunicator.h>
#include <yaml-cpp/node/parse.h>
#include <Game/TBSGame.h>
#include "Configuration/GameConfigParser.h"

int main()
{
	// Read Config
	std::mt19937 engine(0ll);
	SGA::GameConfigParser parser;
	std::filesystem::path configPath2("../../../gameConfigs/KillTheKing.yaml");
	auto yamlConfig2 = YAML::LoadFile(configPath2.string());
	auto gameConfig2 = parser.parseFromFile(configPath2.string());
	auto renderConfig = yamlConfig2.as<SGA::RenderConfig>();
	
	//// Initialize the game
	auto game = generateAbstractGameFromConfig(gameConfig2, engine);
	int playerID = 0;
	int humanPlayerID=-1;
	auto agents = gameConfig2.generateAgents();
	
	const std::uniform_int_distribution<unsigned int> distribution(0,std::numeric_limits<unsigned int>::max());
	for(size_t i = 0; i < gameConfig2.getNumberOfPlayers(); i++)
	{
		auto agent = std::move(agents[i]);
		// This is a human player, treat is as an non existing agent. The Renderer will handle it
		if (agent == nullptr)
		{
			humanPlayerID = playerID;
			playerID++;
			continue;
		}
		if (gameConfig2.gameType == SGA::ForwardModelType::TBS)
		{
			std::unique_ptr<SGA::TBSGameCommunicator> comm = std::make_unique<SGA::TBSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(engine)));
			game->addCommunicator(std::move(comm));
		}
		else
		{
			std::unique_ptr<SGA::RTSGameCommunicator> comm = std::make_unique<SGA::RTSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::RTSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(engine)));
			game->addCommunicator(std::move(comm));			
		}
		
		playerID++;
	}
	
	// Initialize the gameRenderer
	// We change the current_path to load sprites relative to the folder containing the configuration file
	auto tmp = std::filesystem::current_path();
	current_path(absolute(configPath2.parent_path()));
	auto stateRenderer = stateRendererFromConfig(*game, renderConfig, gameConfig2, humanPlayerID);
	current_path(tmp);
	game->addCommunicator(std::move(stateRenderer));
	
	// Run the game
	game->run();
	
    return 0;
}
