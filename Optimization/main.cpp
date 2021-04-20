#include <NTBEA.h>
#include <Evaluators/AbstractGameStateMCTSEvaluator.h>
#include <Evaluators/Evaluator.h>
#include <NTupleLandscapeModel.h>
#include <random>
#include <Mutator.h>

#include <Stratega/Configuration/GameConfigParser.h>

#include <GameRunner.h>
#include <InputParser.h>

#include <filesystem>



int main(int argc, char** argv)
{

	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/TBS/KillTheKing.yaml");
	auto agent = parser.getCmdOption<int>("-agent", 0);

	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if (configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	std::cout << "configPath" << configPath;

	// Read Config
	SGA::GameConfigParser configParser;
	auto gameConfig = configParser.parseFromFile(configPath);
	
	//auto yamlConfig = YAML::LoadFile(configPath);
	//auto gameConfig = yamlConfig.as<SGA::GameConfig>();
	//std::cout << "test2";

	std::unique_ptr<SGA::Evaluator> evaluator;
	switch(agent){
		case 0: evaluator = std::make_unique<SGA::AbstractGameStateMCTSEvaluator>(
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},
			gameConfig.get()
			);
		break;
	}

	if (evaluator != nullptr)
	{		
		//std::unique_ptr<SGA::Evaluator> evaluator = std::make_unique<SGA::NMaxEvaluator>(3, 5);
		SGA::SearchSpace* searchSpace = evaluator->getSearchSpace();
		SGA::Mutator mutator(searchSpace, false, false, 0.1f, true);
		SGA::NTupleLandscapeModel landscapeModel(searchSpace, std::vector<int> {1, 2});
		SGA::NTBEA ntbea(&landscapeModel, evaluator.get(), searchSpace, &mutator, 2, 20, 10);
		std::mt19937 randomGenerator(1);

		ntbea.run(100, randomGenerator);
		return 0;
	} else
	{
		std::cout << "agent parameter needs to be 0" << "-agent=" << agent;
		return -1;
		
	}
	
	

}
