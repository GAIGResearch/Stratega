#include <filesystem>
//#include <GameStateRenderer.h>
#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>
//#include <Game/TBSGameCommunicator.h>
#include <Game/AbstractTBSGameCommunicator.h>
#include <yaml-cpp/node/parse.h>
#include <ForwardModel/AbstractFM/ActionSpaceBase.h>
#include <Representation/AbstractGS/GameState.h>
//#include <ForwardModel/AbstractFM/Preconditions.h>
#include <ForwardModel/AbstractFM/Precondition.h>
#include <ForwardModel/AbstractFM/Effect.h>
#include <Representation/AbstractGS/Player.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <Game/AbstractGame.h>
int main()
{
	// Read Config
	std::mt19937 engine(0ll);
	
	std::filesystem::path configPath("../../../gameConfigs/KillTheKing.yaml");
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	auto renderConfig = yamlConfig.as<SGA::RenderConfig>();
	std::unique_ptr<SGA::TBSGameState2> test = SGA::generateAbstractTBSStateFromConfig(gameConfig,engine);

	
	
	SGA::ActionSpaceBase<SGA::GameState> temp;
	
	SGA::TBSAbstractForwardModel fm;
	test->turnLimit = 100000000000;
	
	////Add precondition
	SGA::FunctionParameter targetResource(0, 0);
	SGA::FunctionParameter lowerBound(50.);
	SGA::HasResource precondition(targetResource, lowerBound);
	SGA::SamePlayer precondition1;

	//Add effect
	SGA::FunctionParameter targetResourceEffect(0, 1);
	SGA::FunctionParameter amountToAddEffect(10.);
	SGA::AddToResource effect(targetResourceEffect, amountToAddEffect);

	
	//Add effect
	
	SGA::EndTurn effect2;

	//Add effect
	SGA::Move moveEffect;

	//Add effect
	SGA::SpawnUnit spawnUnitEffect;

	
	
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
	actionType2.name = "AddHealth";
	actionType2.sourceType = SGA::ActionSourceType::Unit;
	actionType2.preconditions.emplace_back(std::make_unique<SGA::HasResource>(precondition));
	actionType2.condition.emplace_back(std::make_unique<SGA::SamePlayer>(precondition1));
	actionType2.effects.emplace_back(std::make_unique<SGA::AddToResource>(effect));
	//actionType2.effects.emplace_back(std::make_unique<SGA::HasResource>(effect));
	SGA::TargetType target;
	target.type = SGA::TargetType::Entity;
	target.groupEntityTypes.insert(0);

	actionType2.actionTargets = target;
	
	test->actionTypes->emplace(1, std::move(actionType2));

	//Add actionType Move
	SGA::ActionType actionType3;
	actionType3.id = 2;
	actionType3.name = "Move";
	actionType3.sourceType = SGA::ActionSourceType::Unit;
	actionType3.effects.emplace_back(std::make_unique<SGA::Move>(moveEffect));
	SGA::TargetType targetMove;
	targetMove.type = SGA::TargetType::Position;
	targetMove.shapeType = SGA::ShapeType::Square;
	targetMove.shapeSize = 5;
	targetMove.groupEntityTypes.insert(0);

	actionType3.actionTargets = targetMove;

	test->actionTypes->emplace(2, std::move(actionType3));

	//Add actionType Spawn
	SGA::ActionType actionType4;
	actionType4.id = 3;
	actionType4.name = "SpawnUnit";
	actionType4.sourceType = SGA::ActionSourceType::Unit;
	actionType4.effects.emplace_back(std::make_unique<SGA::SpawnUnit>(spawnUnitEffect));
	SGA::TargetType targetSpawn;
	targetSpawn.type = SGA::TargetType::Position;
	targetSpawn.shapeType = SGA::ShapeType::Circle;
	targetSpawn.shapeSize = 3;
	targetSpawn.groupEntityTypes.insert(1);

	actionType4.actionTargets = targetSpawn;

	test->actionTypes->emplace(3, std::move(actionType4));

	// Add EntityType
	SGA::EntityType type;
	type.id = 0;
	type.name = "Unit";
 	test->parameterIDLookup->emplace("Health", 0);
	type.parameters.emplace(0, SGA::Parameter{ "Health", 0, 0, -20, 20 });
	test->entityTypes->emplace(0, std::move(type));

	SGA::EntityType buildingType;
	buildingType.id = 1;
	buildingType.name = "Building";
	test->parameterIDLookup->emplace("Health", 0);
	buildingType.parameters.emplace(0, SGA::Parameter{ "Health", 0, 0, -20, 20 });
	test->entityTypes->emplace(1, std::move(buildingType));

	//Add entity
	SGA::Entity building1;
	building1.id = 3;
	building1.owner = 0;
	building1.actionTypeIds.emplace_back(1);
	building1.actionTypeIds.emplace_back(3);
	building1.position = SGA::Vector2f(15, 10);
	building1.typeID = 1;
	building1.parameters.emplace_back(200);

	test->entities.emplace_back(building1);


	//Add entity
	SGA::Entity building2;
	building2.id = 2;
	building2.owner = 1;
	building2.actionTypeIds.emplace_back(1);
	building2.actionTypeIds.emplace_back(3);
	building2.position = SGA::Vector2f(20, 10);
	building2.typeID = 1;
	building2.parameters.emplace_back(200);

	test->entities.emplace_back(building2);
	
	//Add entity
	SGA::Entity entity;
	entity.id = 0;
	entity.owner = 0;
	entity.actionTypeIds.emplace_back(0);
	entity.actionTypeIds.emplace_back(1);
	entity.actionTypeIds.emplace_back(2);
	entity.position = SGA::Vector2f(10, 10);
	entity.typeID = 0;
	entity.parameters.emplace_back(60);
	
	test->entities.emplace_back(entity);

	SGA::Entity entity2;
	entity2.id = 1;
	entity2.owner = 1;
	entity2.actionTypeIds.emplace_back(0);
	entity2.actionTypeIds.emplace_back(1);
	entity2.actionTypeIds.emplace_back(2);
	entity2.position = SGA::Vector2f(3, 3);
	entity2.typeID = 0;
	entity2.parameters.emplace_back(60);

	test->entities.emplace_back(entity2);

	//GenerateGame
	std::unique_ptr<SGA::AbstractTBSGame>game= std::make_unique<SGA::AbstractTBSGame>(std::move(test),fm,engine);
	


	
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
			std::unique_ptr<SGA::AbstractTBSGameCommunicator> comm = std::make_unique<SGA::AbstractTBSGameCommunicator>(playerID);
			comm->setAgent(std::move(agent));
			comm->setGame(dynamic_cast<SGA::AbstractTBSGame&>(*game));
			comm->setRNGEngine(std::mt19937(distribution(engine)));
			game->addCommunicator(std::move(comm));
		}
		
		playerID++;
	}
	
	// Initialize the gameRenderer
	// We change the current_path to load sprites relative to the folder containing the configuration file
	auto tmp = std::filesystem::current_path();
	current_path(absolute(configPath.parent_path()));
	auto stateRenderer = SGA::stateRendererFromConfig(*game, renderConfig, gameConfig, humanPlayerID);
	current_path(tmp);

	AbstractGameStateRender* render = dynamic_cast<AbstractGameStateRender*>(stateRenderer.get());
	//Register entitytypes


	
	render->entitySpriteTypes->emplace(0, "unit_0");
	render->entitySpriteTypes->emplace(1, "building");
	game->addCommunicator(std::move(stateRenderer));
	
	// Run the game
	
	
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
