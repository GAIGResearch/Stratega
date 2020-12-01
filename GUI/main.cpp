#include <filesystem>
//#include <GameStateRenderer.h>
//#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>
//#include <Game/TBSGameCommunicator.h>
#include <Game/AbstractGameCommunicator.h>
#include <yaml-cpp/node/parse.h>
#include <ForwardModel/AbstractFM/ActionSpaceBase.h>
#include <Representation/AbstractGS/GameState.h>
//#include <ForwardModel/AbstractFM/Preconditions.h>
#include <ForwardModel/AbstractFM/Precondition.h>
#include <ForwardModel/AbstractFM/Effect.h>
#include <Representation/AbstractGS/Player.h>
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <Game/AbstractGame.h>
int main()
{
	SGA::ActionSpaceBase<SGA::GameState> temp;
	std::unique_ptr<SGA::TBSGameState2> test= std::make_unique<SGA::TBSGameState2>();
	SGA::ForwardModel<SGA::TBSGameState2> fm;
	
	////Add precondition
	SGA::FunctionParameter targetResource(0, 0);
	SGA::FunctionParameter lowerBound(50.);
	SGA::HasResource precondition(targetResource, lowerBound);

	//Add effect
	SGA::FunctionParameter targetResourceEffect(0, 0);
	SGA::FunctionParameter amountToAddEffect(10.);
	SGA::AddToResource effect(targetResourceEffect, amountToAddEffect);

	//std::unique_ptr<SGA::AddToResource> effect3 = std::make_unique<SGA::AddToResource>(targetResourceEffect, amountToAddEffect);

	//Add effect
	
	SGA::EndTurn effect2;
	
	
	//Add actionType
	SGA::ActionType actionType;
	actionType.id = 0;
	actionType.name = "EndTurn";
	actionType.sourceType = SGA::ActionSourceType::Player;
	actionType.effects.emplace_back(std::make_unique<SGA::EndTurn>(effect2));

	test->actionTypes->emplace(0, std::move(actionType));

	//Add actionType
	SGA::ActionType actionType2;
	actionType2.id = 1;
	actionType2.name = "Test";
	actionType2.sourceType = SGA::ActionSourceType::Unit;
	actionType2.preconditions.emplace_back(std::make_unique<SGA::HasResource>(precondition));
	actionType2.effects.emplace_back(std::make_unique<SGA::AddToResource>(effect));
	//actionType2.effects.emplace_back(std::make_unique<SGA::HasResource>(effect));
	SGA::TargetType target;
	target.type = SGA::TargetType::None;
	target.groupEntityTypes.insert(0);

	actionType2.actionTargets = target;
	
	//test->actionTypes->emplace(1, std::move(actionType2));
	
	//Add players
	SGA::Player player1;
	player1.canPlay = true;
	player1.id = 0;
	player1.score = 0;
	SGA::Player player2;
	player2.canPlay = true;
	player2.id = 0;
	player2.score = 0;
	
	test->players.emplace_back(player1);
	test->players.emplace_back(player2);

	// Add EntityType
	SGA::EntityType type;
	type.id = 0;
	type.name = "Fuck";
	type.parameterNameIndexLookup.emplace("Health", 0);
	type.parameters.emplace(0, SGA::Parameter{ "Health", 0, 0, -20, 20 });
	test->entityTypes->emplace(0, std::move(type));
	
	//Add entity
	SGA::Entity entity;
	entity.id = 0;
	entity.owner = 0;
	entity.actionTypeIds.emplace_back(0);
	entity.position = SGA::Vector2f(0, 0);
	entity.typeID = 0;
	entity.parameters.emplace_back(60);
	
	test->entities.emplace_back(entity);

	SGA::Entity entity2;
	entity2.id = 1;
	entity2.owner = 1;
	entity2.actionTypeIds.emplace_back(0);
	entity2.position = SGA::Vector2f(1, 1);
	entity2.typeID = 0;
	entity2.parameters.emplace_back(1);

	test->entities.emplace_back(entity2);


	auto actions=fm.generateActions(*test,0);
	std::mt19937 engine(0ll);
	//GenerateGame
	std::unique_ptr<SGA::AbstractGame>game= std::make_unique<SGA::AbstractGame>(std::move(test),fm,engine);
	

	// Read Config
	std::filesystem::path configPath("../../../gameConfigs/KillTheKing.yaml");
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();


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
		if (gameConfig.gameType == "TBS")
		{
			std::unique_ptr<SGA::AbstractGameCommunicator> comm = std::make_unique<SGA::AbstractGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::AbstractGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(engine)));
			game->addCommunicator(std::move(comm));
		}
		
		playerID++;
	}

	game->addActionToExecute(actions[0]);
	game->run();
	
	//std::mt19937 rngEngine(0ll);

	//// Read Config
	//std::filesystem::path configPath("../../../gameConfigs/KillTheKing.yaml");
	//auto yamlConfig = YAML::LoadFile(configPath.string());
	//auto renderConfig = yamlConfig.as<SGA::RenderConfig>();
	//auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	//// Initialize the game
	//auto game = SGA::generateGameFromConfig(gameConfig, rngEngine);
	//
	//int playerID = 0;
	//int humanPlayerID=-1;
	//auto agents = SGA::agentsFromConfig(gameConfig);
	//
	//const std::uniform_int_distribution<unsigned int> distribution(0,std::numeric_limits<unsigned int>::max());
	//for(size_t i = 0; i < gameConfig.getNumberOfPlayers(); i++)
	//{
	//	auto agent = std::move(agents[i]);
	//	// This is a human player, treat is as an non existing agent. The Renderer will handle it
	//	if (agent == nullptr)
	//	{
	//		humanPlayerID = playerID;
	//		playerID++;
	//		continue;
	//	}
	//	
	//	if(gameConfig.gameType == "TBS")
	//	{
	//		std::unique_ptr<SGA::TBSGameCommunicator> comm = std::make_unique<SGA::TBSGameCommunicator>(playerID);
	//		comm->setAgent(std::move(agent));
	//		comm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
	//		comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
	//		game->addCommunicator(std::move(comm));
	//	}
	//	else
	//	{
	//		std::unique_ptr<SGA::RTSGameCommunicator> comm = std::make_unique<SGA::RTSGameCommunicator>(playerID);
	//		comm->setAgent(std::move(agent));
	//		comm->setGame(dynamic_cast<SGA::RTSGame&>(*game));
	//		comm->setRNGEngine(std::mt19937(distribution(rngEngine)));
	//		game->addCommunicator(std::move(comm));			
	//	}
	//	
	//	playerID++;
	//}
	//
	////Build Navmesh
	//if (gameConfig.gameType == "RTS")
	//{
	//	auto& gameRTS = dynamic_cast<SGA::RTSGame&>(*game);
	//	const SGA::RTSForwardModel& fm = gameRTS.getForwardModel();
	//	SGA::RTSGameState& state = *gameRTS.gameState;
	//	fm.buildNavMesh(state,SGA::NavigationConfig());
	//}

	//// Initialize the gameRenderer
	//// We change the current_path to load sprites relative to the folder containing the configuration file
	//auto tmp = std::filesystem::current_path();
	//current_path(absolute(configPath.parent_path()));
	//auto stateRenderer = SGA::stateRendererFromConfig(*game, renderConfig, gameConfig, humanPlayerID);
	//current_path(tmp);

	//game->addCommunicator(std::move(stateRenderer));
	//
	//// Run the game
	//game->run();
	
    return 0;
}
