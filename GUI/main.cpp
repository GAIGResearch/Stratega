#include <filesystem>
#include <AbstractRTSGameStateRender.h>
#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>
//#include <Game/TBSGameCommunicator.h>
#include <Game/AbstractTBSGameCommunicator.h>
#include <yaml-cpp/node/parse.h>
#include <ForwardModel/AbstractFM/ActionSpaceBase.h>
#include <Representation/AbstractGS/GameState.h>
//#include <ForwardModel/AbstractFM/Preconditions.h>
#include <ForwardModel/AbstractFM/Condition.h>
#include <ForwardModel/AbstractFM/Effect.h>
#include <Representation/AbstractGS/Player.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <Game/AbstractGame.h>
int main()
{
	// Read Config
	std::mt19937 engine(0ll);
	
	std::filesystem::path configPath2("../../../gameConfigs/TestConfig.yaml");
	auto yamlConfig2 = YAML::LoadFile(configPath2.string());
	auto gameConfig2 = yamlConfig2.as<SGA::GameConfig>();
	auto renderConfig = yamlConfig2.as<SGA::RenderConfig>();
	
	//// Initialize the game
	auto game = SGA::generateAbstractGameFromConfig(gameConfig2, engine);
	int playerID = 0;
	int humanPlayerID=-1;
	auto agents = SGA::agentsFromConfig(gameConfig2);
	
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
		if (gameConfig2.gameType == "TBS")
		{
			std::unique_ptr<SGA::AbstractTBSGameCommunicator> comm = std::make_unique<SGA::AbstractTBSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::AbstractTBSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(engine)));
			game->addCommunicator(std::move(comm));
		}
		else
		{
			std::unique_ptr<SGA::AbstractRTSGameCommunicator> comm = std::make_unique<SGA::AbstractRTSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::AbstractRTSGame&>(*game));
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
