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
#include <ForwardModel/AbstractFM/Condition.h>
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

	//auto renderConfig = yamlConfig.as<SGA::RenderConfig>();
	std::unique_ptr<SGA::TBSGameState2> test = SGA::generateAbstractTBSStateFromConfig(gameConfig,engine);
	std::unique_ptr<SGA::RTSGameState2> RTSGameState = SGA::generateAbstractRTSStateFromConfig(gameConfig,engine);

	
	
	SGA::ActionSpaceBase<SGA::GameState> temp;
	
	//SGA::TBSAbstractForwardModel fm;
	SGA::RTSAbstractForwardModel fm;
	test->turnLimit = 100000000000;
	fm.winCondition = SGA::WinConditionType::UnitAlive;
	fm.unitTypeID = 1;
	
	
	////Add precondition
	SGA::FunctionParameter targetLifeResource(0, 0);
	SGA::FunctionParameter lowerBound(0.);
	std::vector<SGA::FunctionParameter> parameters;
	parameters.emplace_back(targetLifeResource);
	parameters.emplace_back(lowerBound);
	SGA::HasResource hasLifePrecondition(parameters);

	SGA::FunctionParameter targetGoldResource(1, 0);
	SGA::FunctionParameter lowerGoldBound(10.);
	std::vector<SGA::FunctionParameter> parametersGold;
	parametersGold.emplace_back(targetGoldResource);
	parametersGold.emplace_back(lowerGoldBound);
	SGA::HasResource hasGoldPrecondition(parametersGold);
	
	SGA::SamePlayer precondition1;

	//Add effect
	SGA::FunctionParameter targetResourceEffect(0, 1);
	SGA::FunctionParameter amountToAddEffect(10.);
	std::vector<SGA::FunctionParameter> parametersEffect;
	parametersEffect.emplace_back(targetResourceEffect);
	parametersEffect.emplace_back(amountToAddEffect);
	SGA::AddToResource addHealth(parametersEffect);

	//Remove gold effect
	SGA::FunctionParameter targetGoldResourceEffect(1, 0);
	SGA::FunctionParameter amountToRemoveGoldEffect(10.);
	std::vector<SGA::FunctionParameter> parametersGoldEffect;
	parametersGoldEffect.emplace_back(targetGoldResourceEffect);
	parametersGoldEffect.emplace_back(amountToRemoveGoldEffect);
	SGA::RemoveFromResource removeGoldEffect(parametersGoldEffect);

	//Remove Health effect
	SGA::FunctionParameter targetHealthResourceEffect(0, 1);
	SGA::FunctionParameter targetHealthEffect(1);
	SGA::FunctionParameter amountToRemoveHealthEffect(20.);
	std::vector<SGA::FunctionParameter> parametersHealthEffect;
	parametersHealthEffect.emplace_back(targetHealthEffect);
	parametersHealthEffect.emplace_back(targetHealthResourceEffect);
	parametersHealthEffect.emplace_back(amountToRemoveHealthEffect);
	//SGA::RemoveFromResource removeHealthEffect(parametersHealthEffect);

	//Attack effect	
	SGA::Attack attackEffect(parametersHealthEffect);

	
	//Add effect
	/*
	SGA::EndTurn effect2({});*/

	//Add effect
	SGA::Move moveEffect({});

	//Add effect
	SGA::SpawnUnit spawnUnitEffect;

	
	
	//Add actionType
	SGA::ActionType actionType;
	actionType.id = 0;
	actionType.name = "EndTurn";
	actionType.sourceType = SGA::ActionSourceType::Player;

	RTSGameState->actionTypes->emplace(0, std::move(actionType));

	//Add actionType
	SGA::ActionType actionType2;
	actionType2.id = 1;
	actionType2.name = "AddHealth";
	actionType2.sourceType = SGA::ActionSourceType::Unit;
	actionType2.preconditions.emplace_back(std::make_unique<SGA::HasResource>(hasLifePrecondition));
	actionType2.condition.emplace_back(std::make_unique<SGA::SamePlayer>(precondition1));
	actionType2.effects.emplace_back(std::make_unique<SGA::AddToResource>(addHealth));
	
	SGA::TargetType target;
	target.type = SGA::TargetType::Entity;
	target.groupEntityTypes.insert(0);

	actionType2.actionTargets = target;
	
	RTSGameState->actionTypes->emplace(1, std::move(actionType2));

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

	RTSGameState->actionTypes->emplace(2, std::move(actionType3));

	//Add actionType Spawn
	SGA::ActionType actionType4;
	actionType4.id = 3;
	actionType4.name = "SpawnUnit";
	actionType4.sourceType = SGA::ActionSourceType::Unit;
	actionType4.effects.emplace_back(std::make_unique<SGA::SpawnUnit>(spawnUnitEffect));
	actionType4.effects.emplace_back(std::make_unique<SGA::RemoveFromResource>(removeGoldEffect));
	actionType4.preconditions.emplace_back(std::make_unique<SGA::HasResource>(hasGoldPrecondition));
	SGA::TargetType targetSpawn;
	targetSpawn.type = SGA::TargetType::Position;
	targetSpawn.shapeType = SGA::ShapeType::Circle;
	targetSpawn.shapeSize = 3;
	targetSpawn.groupEntityTypes.insert(1);

	actionType4.actionTargets = targetSpawn;

	RTSGameState->actionTypes->emplace(3, std::move(actionType4));

	SGA::ActionType actionType5;
	actionType5.id = 4;
	actionType5.name = "Attack";
	actionType5.sourceType = SGA::ActionSourceType::Unit;
	actionType5.preconditions.emplace_back(std::make_unique<SGA::HasResource>(hasLifePrecondition));
	actionType5.effects.emplace_back(std::make_unique<SGA::Attack>(attackEffect));
	//actionType2.effects.emplace_back(std::make_unique<SGA::HasResource>(effect));
	SGA::TargetType targetHealth;
	targetHealth.type = SGA::TargetType::Entity;
	targetHealth.groupEntityTypes.insert(0);
	targetHealth.groupEntityTypes.insert(1);

	actionType5.actionTargets = targetHealth;

	RTSGameState->actionTypes->emplace(4, std::move(actionType5));

	// Add EntityType
	SGA::EntityType type;
	type.id = 0;
	type.name = "Unit";
	type.parameters.emplace(0, SGA::Parameter{ "Health", 0, 0, -20, 20 });
	type.parameters.emplace(1, SGA::Parameter{ "Gold", 1, 0, 0, 100 });
	RTSGameState->entityTypes->emplace(0, std::move(type));

	SGA::EntityType buildingType;
	buildingType.id = 1;
	buildingType.name = "Building";
	buildingType.parameters.emplace(0, SGA::Parameter{ "Health", 0, 50, -20, 20 });
	buildingType.parameters.emplace(1, SGA::Parameter{ "Gold", 1, 50, 0, 100 });
	RTSGameState->entityTypes->emplace(1, std::move(buildingType));



	RTSGameState->parameterIDLookup->emplace("Health", 0);
	RTSGameState->parameterIDLookup->emplace("Gold", 1);
	//Add entity
	SGA::Entity building1;
	building1.id = 3;
	building1.owner = 0;
	/*building1.actionTypeIds.emplace_back(1);*/
	building1.actionTypeIds.emplace_back(3);
	building1.position = SGA::Vector2f(15, 10);
	building1.typeID = 1;
	building1.parameters.emplace_back(200);
	building1.parameters.emplace_back(10);

	RTSGameState->entities.emplace_back(building1);


	//Add entity
	SGA::Entity building2;
	building2.id = 2;
	building2.owner = 1;
	/*building2.actionTypeIds.emplace_back(1);*/
	building2.actionTypeIds.emplace_back(3);
	building2.position = SGA::Vector2f(20, 10);
	building2.typeID = 1;
	building2.parameters.emplace_back(200);
	building2.parameters.emplace_back(10);

	RTSGameState->entities.emplace_back(building2);
	
	//Add entity
	SGA::Entity entity;
	entity.id = 0;
	entity.owner = 0;
	entity.actionTypeIds.emplace_back(0);
	//entity.actionTypeIds.emplace_back(1);
	entity.actionTypeIds.emplace_back(2);
	entity.actionTypeIds.emplace_back(4);
	entity.position = SGA::Vector2f(10, 10);
	entity.typeID = 0;
	entity.parameters.emplace_back(60);
	entity.parameters.emplace_back(0);
	
	RTSGameState->entities.emplace_back(entity);

	SGA::Entity entity2;
	entity2.id = 1;
	entity2.owner = 1;
	entity2.actionTypeIds.emplace_back(0);
	//entity2.actionTypeIds.emplace_back(1);
	entity2.actionTypeIds.emplace_back(2);
	entity2.actionTypeIds.emplace_back(4);
	entity2.position = SGA::Vector2f(3, 3);
	entity2.typeID = 0;
	entity2.parameters.emplace_back(60);
	entity2.parameters.emplace_back(0);

	RTSGameState->entities.emplace_back(entity2);

	//GenerateGame
	std::unique_ptr<SGA::AbstractRTSGame>game= std::make_unique<SGA::AbstractRTSGame>(std::move(RTSGameState),fm,engine);
	


	
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
	current_path(absolute(configPath.parent_path()));
	//auto stateRenderer = SGA::stateRendererFromConfig(*game, renderConfig, gameConfig, humanPlayerID);
	current_path(tmp);

	//AbstractGameStateRender* render = dynamic_cast<AbstractGameStateRender*>(stateRenderer.get());
	//Register entitytypes
	/*render->entitySpriteTypes->emplace(0, "unit_0");
	render->entitySpriteTypes->emplace(1, "building");
	game->addCommunicator(std::move(stateRenderer));*/
	
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
