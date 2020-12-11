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

int main(int argc, char **argv)
{
    std::string filename = "../../../gameConfigs/KillTheKing.yaml";
    if(argc > 1)
        filename = argv[1];
    else
        std::cout << "Loading default config " << filename << std::endl;

    std::filesystem::path configPath(filename);
	std::mt19937 rngEngine(0ll);

	// Read Config
	auto yamlConfig = YAML::LoadFile(configPath.string());
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	std::cout << "Passed game is a " << gameConfig.gameType << " game" << std::endl;
	
	// Run "tests"
	TestGameStateCopy(*SGA::generateTBSStateFromConfig(gameConfig, rngEngine));
	
	FMEvaluator evaluator(rngEngine);
	auto results = evaluator.evaluate(gameConfig);
	std::cout << "FPS: " << results->computeFPS() << std::endl;
	std::cout << "FPS: " << results->computeFPS() << std::endl;
}