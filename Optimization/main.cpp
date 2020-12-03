#include <NTBEA.h>
#include <examples/NMaxEvaluator.h>
#include <RHEAEvaluator.h>
#include <NTupleLandscapeModel.h>
#include <random>


#include <Configuration/GameConfig.h>
#include <Logging/Log.h>

#include <GameRunner.h>
#include <InputParser.h>

#include <filesystem>


int main(int argc, char** argv)
{

	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/KillTheKing.yaml");
	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if (configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}

	// Read Config
	auto yamlConfig = YAML::LoadFile(configPath);
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();
	
	
	const std::unique_ptr<SGA::Evaluator> evaluator = std::make_unique<SGA::RHEAEvaluator>(
		std::vector<int> {1, 10, 100},
		std::vector<int> {1, 3, 5, 10},
		std::vector<float> {0.1, 0.5, 0.9},
		std::vector<int> {3, 5, 10},
		std::vector<bool> {false, true},
		std::vector<bool> {false, true},
		gameConfig
		);
	//std::unique_ptr<SGA::Evaluator> evaluator = std::make_unique<SGA::NMaxEvaluator>(3, 5);
	SGA::SearchSpace* searchSpace = evaluator->getSearchSpace();
	SGA::Mutator mutator(searchSpace, false, false, 0.1f, true);
	SGA::NTupleLandscapeModel landscapeModel(searchSpace, std::vector<int> {1, 2});
	SGA::NTBEA ntbea(&landscapeModel, evaluator.get(), searchSpace, &mutator, 2, 5, 50);
	std::mt19937 randomGenerator(1);

	ntbea.run(100, randomGenerator);
	return 0;

}
