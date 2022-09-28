#include <Stratega/ntbea/Evaluators/MCTSuEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	MCTSuEvaluator::MCTSuEvaluator(
		std::vector<float> k_values,		// values of k
		std::vector<int> rollout_length,	// rollout length
		//std::vector<int> opponent_script,	// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
        float fm,
        std::string heuristic,
		GameConfig& configInput
	)
	: Evaluator("UnitMCTSEvaluator"),
		k_values(k_values), rollout_length(rollout_length),
        fm(fm), heuristic(heuristic),
		agents(configInput.generateAgents()), config(configInput)
    {
        arena = std::make_unique<Arena>(config);
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(k_values.size());
		searchSpaceDims.emplace_back(rollout_length.size());
		//searchSpaceDims.emplace_back(opponent_script.size());

		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config.numPlayers = 2;
    }


	std::vector<float> MCTSuEvaluator::evaluate(std::vector<int> point, int nSamples)
	{
		nSamples = config.levelDefinitions.size();
		float value = 0;

		float agentValue = 0;
		float nrOfWins = 0;
		bool playFirst = false;
		std::cout << "evaluate agent: ";

		currentPoint = point;
        auto ss = generateAgents();
        /*std::cout << "agent size: " << ss.size() << "\n";
        for (int i = 0; i < ss.size(); i++) {
            if (ss[i] == nullptr) {
                std::cout << "NULL agent!\n";
            }
            else {
                std::cout<<ss[i]->getName()<<"\n";
            }
        }*/
        //std::cout<<"jij"<<std::endl;

		auto results = arena->runGames([&]() {return generateAgents(); }, 2, 0, 1, config.levelDefinitions.size());
		agentValue = results[0];
		nrOfWins = results[1];

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}


	void MCTSuEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "K=" << (k_values[point[0]]) << ", ";
		std::cout << "RL=" << (rollout_length[point[1]]) << ", ";
		//std::cout << "OS=" << (point[2]) << ";";
    }

	std::vector<std::unique_ptr<Agent>> MCTSuEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();;

		auto allAgents = config.generateAgents();

		// setup current agent configuration
		UnitMCTSParameters params;

        params.budgetType = SGA::Budget::FMCALLS;
        params.maxFMCalls = fm;
        params.HEURISTIC = heuristic;

		params.K = k_values[currentPoint[0]];
		params.ROLLOUT_LENGTH = rollout_length[currentPoint[1]];

		/*switch (currentPoint[2])
		{
		case 0:
			params.OPPONENT_MODEL = std::make_unique<AttackClosestOpponentScript>();;
			break;
		case 1:
			params.OPPONENT_MODEL = std::make_unique<AttackWeakestOpponentScript>();;
			break;
		case 2:
			params.OPPONENT_MODEL = std::make_unique<RandomActionScript>();;
			break;
		default:
			params.OPPONENT_MODEL = nullptr;
			break;
		}
        */

		allAgents[0] = std::make_unique<UnitMCTSAgent>("UnitMCTSAgent", std::move(params));
		return allAgents;
	}
}
