#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "FMEvaluator.h"

#include <Configuration/GameConfig.h>


void TestGameStateCopy(const SGA::TBSGameState& state)
{
	auto copy = state;
	for(auto& player : copy.getPlayers())
	{
		if (&player.state.get() != &copy)
		{
			std::cout << "Error Player" << std::endl;
		}
	}

	for (auto& unit : copy.getUnits())
	{
		if (&unit.state.get() != &copy)
		{
			std::cout << "Error Unit" << std::endl;
		}
	}

	if(&copy.getTileTypes() != &state.getTileTypes())
	{
		std::cout << "Error tile-types point to different addresses" << std::endl;
	}
	
	if (&copy.getUnitTypes() != &state.getUnitTypes())
	{
		std::cout << "Error unit-types point to different addresses" << std::endl;
	}
}

void main()
{
	std::mt19937 rngEngine(0ll);

	// Read Config
	std::filesystem::path configPath("../../../gameConfigs/KillTheKing.yaml");
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	// Generate objects from the config
	auto fm = SGA::forwardModelFromConfig(gameConfig);

	// Run "tests"
	TestGameStateCopy(*SGA::generateTBSStateFromConfig(gameConfig, rngEngine));
	
	FMEvaluator evaluator(rngEngine);
	auto results = evaluator.evaluate(gameConfig, fm);
	std::cout << "FPS: " << results->computeFPS() << std::endl;
}