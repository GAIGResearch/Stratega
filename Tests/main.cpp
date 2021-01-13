#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "FMEvaluator.h"

#include <Configuration/GameConfig.h>
#include "Configuration/GameConfigParser.h"

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
	SGA::GameConfigParser parser;
	auto gameConfig = parser.parseFromFile(configPath.string());

	std::cout << "Passed game is a " << (gameConfig.gameType == SGA::ForwardModelType::TBS ? "TBS" : "RTS") << " game" << std::endl;
	
	FMEvaluator evaluator(rngEngine);
	auto results = evaluator.evaluate(gameConfig);
	std::cout << "FPS: " << results->computeFPS() << std::endl;
}
