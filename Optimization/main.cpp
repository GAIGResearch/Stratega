#include <NTBEA.h>
#include <Evaluators/AbstractGameStateMCTSEvaluator.h>
#include <Evaluators/ActionAbstractingMCTSEvaluator.h>
#include <Evaluators/AbstractingEverythingMCTSEvaluator.h>
#include <Evaluators/MCTSEvaluator.h>

#include <Evaluators/Evaluator.h>
#include <NTupleLandscapeModel.h>
#include <random>
#include <Mutator.h>

#include <Stratega/Configuration/GameConfigParser.h>

#include <InputParser.h>

#include <filesystem>



int main(int argc, char** argv)
{

	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/TBS/KillTheKing.yaml");
	auto agent = parser.getCmdOption<int>("-agent", 1);
	auto mapsPath = parser.getCmdOption<std::string>("-mapsPath", "../../../gameConfigs/TBS/KtKMaps_training.yaml");

	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if (configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	std::cout << "configPath: " << configPath << std::endl;


	// Read Config
	SGA::GameConfigParser configParser;
	auto gameConfig = configParser.parseFromFile(configPath);
	std::cout << "parsed Config file:" << std::endl;
	
	if (!mapsPath.empty())
	{
		gameConfig->levelDefinitions = SGA::loadLevelsFromYAML(mapsPath, *gameConfig);
	}
	std::cout << "number of maps: " << gameConfig->levelDefinitions.size() << std::endl;

	//auto yamlConfig = YAML::LoadFile(configPath);
	//auto gameConfig = yamlConfig.as<SGA::GameConfig>();
	//std::cout << "test2";

	std::unique_ptr<SGA::Evaluator> evaluator;
	switch(agent){
	case 1: 
		std::cout << "Optimize MCTS Agent" << std::endl;
		evaluator = std::make_unique<SGA::MCTSEvaluator>(
			std::vector<float> {0.1, 1, 10, 100},			// values of k
			std::vector<int> {1, 5, 10, 15, 20, 25, 30},	// rollout length
		    std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
			//std::vector<float> {0, 1, 5},					// magnitude values for each parameter
			//std::vector<float> {0.3, 1, 3},				// u-values for each parameter
			*gameConfig										// gameconfig to determine the list of parameters and run games
			);
			break;
		case 2: 
			std::cout << "Optimize ActionAbstractingMCTS Agent" << std::endl;
			evaluator = std::make_unique<SGA::ActionAbstractingMCTSEvaluator>(
			std::vector<float> {0.1, 1, 10, 100},			// values of k
			std::vector<int> {1, 5, 10, 15, 20, 25, 30},	// rollout length
			std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
			std::vector<bool> {false, true},				// Attack Closest
			std::vector<bool> {false, true},				// Attack Weakest
			std::vector<bool> {false, true},				// Run Away From Opponent
			std::vector<bool> {false, true},				// Run to Friendly Unit
			std::vector<bool> {false, true},				// Use Special Ability
			std::vector<bool> {false, true},				// Random Script
			* gameConfig									// gameconfig to determine the list of parameters and run games
			);	
			break;
		
		case 3: 
			std::cout << "Optimize AbstractGameStateMCTS Agent" << std::endl;
			evaluator = std::make_unique<SGA::AbstractGameStateMCTSEvaluator>(
			std::vector<float> {0.1, 1, 10, 100},			// values of k
			std::vector<int> {1, 5, 10, 15, 20, 25, 30},	// rollout length
			std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
			std::vector<bool> {false, true},				// abstract map
			std::vector<bool> {false, true},				// abstract positions
			* gameConfig									// gameconfig to determine the list of parameters and run games
			);
			break;
		
		case 4: 
			std::cout << "Optimize AbstractingEverythingMCTS Agent" << std::endl;
			
			evaluator = std::make_unique<SGA::AbstractingEverythingMCTSEvaluator>(
			std::vector<float> {0.1, 1, 10, 100},			// values of k
			std::vector<int> {1, 5, 10, 15, 20, 25, 30},	// rollout length
			std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
			std::vector<bool> {false, true},				// Attack Closest
			std::vector<bool> {false, true},				// Attack Weakest
			std::vector<bool> {false, true},				// Run Away From Opponent
			std::vector<bool> {false, true},				// Run to Friendly Unit
			std::vector<bool> {false, true},				// Use Special Ability
			std::vector<bool> {false, true},				// Random Script
			std::vector<bool> {false, true},				// abstract map
			std::vector<bool> {false, true},				// abstract positions
			* gameConfig									// gameconfig to determine the list of parameters and run games
			);
			break;
		
	}

	if (evaluator != nullptr)
	{		
		std::cout << "Start optimization" << std::endl;

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
