#include <NTBEA.h>
#include <Evaluators/NMaxEvaluator.h>
#include <Evaluators/RHEAEvaluator.h>
#include <Evaluators/PortfolioRHEAEvaluator.h>
#include <Evaluators/POEEvaluator.h>
#include <Evaluators/PGSEvaluator.h>
#include <Evaluators/MOPortfolioRHEAEvaluator.h>
#include <Evaluators/MetaPortfolioRHEA.h>
#include <NTupleLandscapeModel.h>
#include <random>
#include <Mutator.h>


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
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/KillTheKingOptimization.yaml");
	auto agent = parser.getCmdOption<int>("-agent", 2);

	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if (configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	std::cout << "configPath" << configPath;

	// Read Config
	auto yamlConfig = YAML::LoadFile(configPath);
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();
	std::cout << "test2";

	std::unique_ptr<SGA::Evaluator> evaluator;
	switch(agent){
		case 0: evaluator = std::make_unique<SGA::RHEAEvaluator>(
			std::vector<int> {1, 10, 100},
			std::vector<int> {1, 3, 5, 10},
			std::vector<float> {0.1, 0.5, 0.9},
			std::vector<int> {3, 5, 10},
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},
			gameConfig
			);
		break;
		
		case 1: evaluator = std::make_unique<SGA::PortfolioRHEAEvaluator>(
			std::vector<int> {1, 10, 100},
			std::vector<int> {1, 3, 5, 10},
			std::vector<float> {0.1, 0.5, 0.9},
			std::vector<int> {3, 5, 10},
			std::vector<bool> {false, true},	//elitism
			std::vector<bool> {false, true},	//continue search
			std::vector<bool> {false, true},	//p1
			std::vector<bool> {false, true},	//p2
			std::vector<bool> {false, true},	//p3
			std::vector<bool> {false, true},	//p4
			std::vector<bool> {false, true},	//p5
			std::vector<bool> {false, true},	//p6
			gameConfig
			);
		break;
		
		case 2: evaluator = std::make_unique<SGA::PGSEvaluator>(
			std::vector<bool> {false, true},	//p1
			std::vector<bool> {false, true},	//p2
			std::vector<bool> {false, true},	//p3
			std::vector<bool> {false, true},	//p4
			std::vector<bool> {false, true},	//p5
			std::vector<bool> {false, true},	//p6
			std::vector<int> {1, 2, 3, 4, 5},	//iterations
			std::vector<int> {1, 2, 3, 4, 5},	//length
			gameConfig
			);
			break;

		case 3: evaluator = std::make_unique<SGA::MOPortfolioRHEAEvaluator>(
			std::vector<int> {1, 10, 100},
			std::vector<int> {1, 3, 5, 10},
			std::vector<float> {0.1, 0.5, 0.9},
			std::vector<int> {3, 5, 10},
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},	//p1
			std::vector<bool> {false, true},	//p2
			std::vector<bool> {false, true},	//p3
			std::vector<bool> {false, true},	//p4
			std::vector<bool> {false, true},	//p5
			std::vector<bool> {false, true},	//p6
			gameConfig
			);
			break;

		case 4: evaluator = std::make_unique<SGA::MetaPortfolioRHEAEvaluator>(
			std::vector<int> {1, 10, 100},
			std::vector<int> {1, 3, 5, 10},
			std::vector<float> {0.1, 0.5, 0.9},
			std::vector<int> {3, 5, 10},
			std::vector<bool> {false, true},	//elitism
			std::vector<bool> {false, true},	//continue search
			std::vector<int> {1, 3, 5, 10},				//nrOfChanges
			std::vector<bool> {false, true},	//p1
			std::vector<bool> {false, true},	//p2
			std::vector<bool> {false, true},	//p3
			std::vector<bool> {false, true},	//p4
			std::vector<bool> {false, true},	//p5
			std::vector<bool> {false, true},	//p6
			gameConfig
			);
			break;

		case 5: evaluator = std::make_unique<SGA::POEEvaluator>(
			std::vector<int> {1, 10, 100},
			std::vector<int> {1, 3, 5, 10},
			std::vector<float> {0.1, 0.5, 0.9},
			std::vector<int> {3, 5, 10},
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},
			std::vector<bool> {false, true},	//p1
			std::vector<bool> {false, true},	//p2
			std::vector<bool> {false, true},	//p3
			std::vector<bool> {false, true},	//p4
			std::vector<bool> {false, true},	//p5
			std::vector<bool> {false, true},	//p6
			gameConfig
			);
			break;
		default: evaluator = nullptr;
			break;

	}
	std::cout << "test3";

	if (evaluator != nullptr)
	{
		std::cout << "start";
		
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
		std::cout << "agent parameter needs to be in the range from [0,5]" << "-agent=" << agent;
		return -1;
		
	}
	
	

}