#include <filesystem>
#include <Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Game/AgentGameCommunicator.h>
#include <Stratega/Configuration/YamlHeaders.h>
#include <Stratega/Game/TBSGame.h>
#include <Stratega/Configuration/GameConfigParser.h>

#include <GameStateRenderer.h>

int main()
{
	// Read Config
	std::mt19937 engine(0ll);
	SGA::GameConfigParser parser;
	std::filesystem::path configPath("../../../gameConfigs/TBS/KillTheKing.yaml");
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto gameConfig = parser.parseFromFile(configPath.string());
	auto renderConfig = yamlConfig.as<SGA::RenderConfig>();
	
	//// Initialize the game
	auto game = SGA::generateAbstractGameFromConfig(gameConfig, engine);
	int playerID = 0;
	int humanPlayerID=-1;
	auto agents = gameConfig.generateAgents();
	
	std::uniform_int_distribution<unsigned int> distribution(0,std::numeric_limits<unsigned int>::max());

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
	
		if (gameConfig.gameType == SGA::GameType::TBS)
		{
			std::unique_ptr<SGA::AgentGameCommunicator> comm = std::make_unique<SGA::AgentGameCommunicator>(playerID,
				dynamic_cast<SGA::TBSGame&>(*game),
				std::move(agent),
				std::mt19937(distribution(engine)));
			
			game->addCommunicator(std::move(comm));
		}
		else
		{
			std::unique_ptr<SGA::AgentGameCommunicator> comm = std::make_unique<SGA::AgentGameCommunicator>(playerID,
				dynamic_cast<SGA::RTSGame&>(*game),
				std::move(agent),
				std::mt19937(distribution(engine)));
			
			game->addCommunicator(std::move(comm));
		}		
		
		playerID++;
	}
	
	// Initialize the gameRenderer
	// We change the current_path to load sprites relative to the folder containing the configuration file
	auto tmp = std::filesystem::current_path();
	current_path(absolute(configPath.parent_path()));
	auto stateRenderer = std::shared_ptr<SGA::GameStateRenderBase>(stateRendererFromConfig(*game, renderConfig, gameConfig, humanPlayerID));
	
	current_path(tmp);
	game->addCommunicator(stateRenderer);
	
	// Run the game
	std::thread gameThread(&SGA::Game::run, std::ref(*game));
	stateRenderer->render();
	gameThread.join();
	
    return 0;
}
