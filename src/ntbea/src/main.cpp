#include <Stratega/ntbea/ntbea.h>
//#include <Stratega/ntbea/Evaluators/AbstractGameStateMCTSEvaluator.h>
//#include <Stratega/ntbea/Evaluators/ActionAbstractingMCTSEvaluator.h>
//#include <Stratega/ntbea/Evaluators/AbstractingEverythingMCTSEvaluator.h>
#include <Stratega/ntbea/Evaluators/MCTSEvaluator.h>
#include <Stratega/ntbea/Evaluators/MCTSuEvaluator.h>
#include <Stratega/ntbea/Evaluators/MCTSuEvaluator.h>
#include <Stratega/ntbea/Evaluators/ElasMCTSuEvaluator.h>
#include <Stratega/ntbea/Evaluators/RandElasMCTSuEvaluator.h>
#include <Stratega/ntbea/Evaluators/NaiveMCTSEvaluator.h>

#include <Stratega/ntbea/Evaluators/Evaluator.h>
#include <Stratega/ntbea/NTupleLandscapeModel.h>
#include <random>
#include <Stratega/ntbea/Mutator.h>

#include <Stratega/Configuration/GameConfigParser.h>

#include <Stratega/ntbea/InputParser.h>

#include <filesystem>

/*Issues met when transfer this to the new version
* 1)pass the incorrect parameters to rungame and cause nullptr agents
* 2)yaml file path incorrect and waste a lot of time on suspecting the code
*/


int main(int argc, char** argv)
{
    InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "../ktk_mcts_ntbea_log.yaml");
	//auto configPath = parser.getCmdOption<std::string>("-configPath", "../resources/gameConfigurations/TBS/ktk_mcts_ntbea_config.yaml");
    auto configPath = parser.getCmdOption<std::string>("-configPath", "../resources/gameConfigurations/TBS/Pusher_naivemcts_rule.yaml");
	auto agent = parser.getCmdOption<int>("-agent", 5);
	//auto mapsPath = parser.getCmdOption<std::string>("-mapsPath", "../resources/gameConfigurations/TBS/ktk_ntbea_map.yaml");
    auto mapsPath = parser.getCmdOption<std::string>("-mapsPath", "../resources/gameConfigurations/TBS/pta_ntbea_map.yaml");
    auto fm = parser.getCmdOption<float>("-fm", 10000);
    auto heuristic = parser.getCmdOption<std::string>("-heuristic", "pta"); // pta || ktk

    // Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if (configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	std::cout << "configPath: " << configPath << std::endl;


	// Read Config
	SGA::GameConfigParser configParser;
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	std::cout << "parsed Config file:" << std::endl;
	
	if (!mapsPath.empty())
	{
        gameConfig->levelDefinitions = SGA::loadLevelsFromYAML(mapsPath, *gameConfig);
	}
	std::cout << "number of maps: " << gameConfig->levelDefinitions.size() << std::endl;

	//auto yamlConfig = YAML::LoadFile(configPath);
	//auto gameConfig = yamlConfig.as<SGA::GameConfig>();
    std::vector<float> r_thresholds = {};
    std::vector<float> t_thresholds = {};

	std::unique_ptr<SGA::Evaluator> evaluator;
	switch(agent){
		case 1: 
			std::cout << "Optimize MCTS Agent" << std::endl;
			evaluator = std::make_unique<SGA::MCTSEvaluator>(
				//std::vector<float> {0.1, 1, 10, 100},			// values of k
				std::vector<float> {0.1, 1, 10, 100},
				//std::vector<int> {1, 5, 10, 15, 20, 25, 30},	// rollout length
				std::vector<int> { 10, 20, 40},
				//std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
				//std::vector<int> {2},
				//std::vector<float> {0, 1, 5},					// magnitude values for each parameter
				//std::vector<float> {0.3, 1, 3},				// u-values for each parameter
                fm,
                heuristic,
				*gameConfig										// gameconfig to determine the list of parameters and run games
			);
            std::cout << "Finished the definition of MCTS Evaluator" << std::endl;
			break;
        case 2: 
			std::cout << "Optimize MCTSu Agent" << std::endl;
			evaluator = std::make_unique<SGA::MCTSuEvaluator>(
				std::vector<float> {0.1, 1, 10, 100},       // values of k
				std::vector<int> { 10, 20, 40},        // rollout length
                fm,
                heuristic,
				//std::vector<int> {0, 1, 2, 3},					// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
				//std::vector<int> {2},
				//std::vector<float> {0, 1, 5},					// magnitude values for each parameter
				//std::vector<float> {0.3, 1, 3},				// u-values for each parameter
				*gameConfig										// gameconfig to determine the list of parameters and run games
			);
            std::cout << "Finished the definition of MCTSu Evaluator" << std::endl;
			break;
        case 3: 
			std::cout << "Optimize Elastic MCTSu Agent" << std::endl;

            if (heuristic == "ktk") {
                r_thresholds.push_back(0.05);
                t_thresholds.push_back(1.0);
            }
            else {
                r_thresholds.push_back(0.1);
                t_thresholds.push_back(2.0);
            }
			evaluator = std::make_unique<SGA::ElasMCTSuEvaluator>(
				std::vector<float> {0.1, 1, 10, 100},               // values of k
				std::vector<int> {  10, 20, 40},            // values of rollout
				//std::vector<float> {0, 0.05, 0.1, 0.3, 0.5, 1.0},     // R threshold
                //std::vector<float> {0, 0.5, 1.0, 1.5, 2.0},     // T threshold
                r_thresholds, t_thresholds,
                std::vector<int> {8},     // earlyStop
				//std::vector<int> {2},
				//std::vector<float> {0, 1, 5},					// magnitude values for each parameter
				//std::vector<float> {0.3, 1, 3},				// u-values for each parameter
                fm,
                heuristic,
				*gameConfig										// gameconfig to determine the list of parameters and run games
			);
            std::cout << "Finished the definition of ElasMCTS Evaluator" << std::endl;
			break;
        case 4: 
			std::cout << "Optimize Random Elastic MCTSu Agent" << std::endl;
			evaluator = std::make_unique<SGA::RandElasMCTSuEvaluator>(
				std::vector<float> {0.1, 1, 10, 100},               // values of k
				std::vector<int> {  10, 20, 40},            // values of rollout
                std::vector<int> {4, 6, 8, 10, 12},     // earlyStop
				//std::vector<int> {2},
				//std::vector<float> {0, 1, 5},					// magnitude values for each parameter
				//std::vector<float> {0.3, 1, 3},				// u-values for each parameter
                fm,
                heuristic,
				*gameConfig										// gameconfig to determine the list of parameters and run games
			);
            std::cout << "Finished the definition of ElasMCTS Evaluator" << std::endl;
			break;
		case 5:
			std::cout << "Optimize Naive MCTS Agent" << std::endl;
			evaluator = std::make_unique<SGA::NaiveMCTSEvaluator>(
				std::vector<int> { 10, 20, 40},            // values of rollout
				std::vector<float> {0.7, 0.9},     // epsilon_global
				std::vector<float> {0.3, 0.5, 0.7},     // epsilon_global
				fm,
				heuristic,
				* gameConfig										// gameconfig to determine the list of parameters and run games
				);
			std::cout << "Finished the definition of Naive MCTS Evaluator" << std::endl;
			break;
    }

	if (evaluator != nullptr)
	{
		std::cout << "Start optimization" << std::endl;

		//std::unique_ptr<SGA::Evaluator> evaluator = std::make_unique<SGA::NMaxEvaluator>(3, 5);
		SGA::SearchSpace* searchSpace = evaluator->getSearchSpace();
		SGA::Mutator mutator(searchSpace, false, false, 0.1f, true);
		SGA::NTupleLandscapeModel landscapeModel(searchSpace, std::vector<int> {1, 2});
		SGA::NTBEA ntbea(&landscapeModel, evaluator.get(), searchSpace, &mutator, 2, 20, 50); // number of neibour could be higher 50
		std::mt19937 randomGenerator(1);

		ntbea.run(50, randomGenerator);
		return 0;
	} else
	{
		std::cout << "agent parameter needs to be a value in the range of [1, 4]" << "-agent=" << agent;
		return -1;
		
	}
    std::cout<<"new main\n";
}