#include <Stratega/ntbea/Evaluators/UGEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	UGEvaluator::UGEvaluator(
		std::vector<float> k_values,		// values of k
		std::vector<int> rollout_length,	// rollout length
        std::vector<float> R_T,
        std::vector<float> T_T,
		//std::vector<int> opponent_script,	// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
        float fm,
        std::string heuristic,
		GameConfig& configInput
	)
	: Evaluator("UGEvaluator"),
		k_values(k_values), rollout_length(rollout_length),
        reward_threshold(R_T), transition_threshold(T_T),
        fm(fm), heuristic(heuristic),
		agents(configInput.generateAgents()), config(configInput)
    {
        arena = std::make_unique<Arena>(config);
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(k_values.size());
		searchSpaceDims.emplace_back(rollout_length.size());
        searchSpaceDims.emplace_back(reward_threshold.size());
        searchSpaceDims.emplace_back(transition_threshold.size());

		//searchSpaceDims.emplace_back(opponent_script.size());

		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config.numPlayers = 2;
    }


	std::vector<float> UGEvaluator::evaluate(std::vector<int> point, int nSamples)
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
        }
        */

		auto results = arena->runGames([&]() {return generateAgents(); }, 2, 0, 1, config.levelDefinitions.size());
		agentValue = results[0];
		nrOfWins = results[1];

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}


	void UGEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "K=" << (k_values[point[0]]) << ", ";
		std::cout << "RL=" << (rollout_length[point[1]]) << ", ";
        std::cout << "R_T=" << (reward_threshold[point[2]]) << ", ";
        std::cout << "T_T=" << (transition_threshold[point[3]]) << ", ";
		//std::cout << "OS=" << (point[2]) << ";";
    }

	std::vector<std::unique_ptr<Agent>> UGEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();

		auto allAgents = config.generateAgents();

		// setup current agent configuration
		UnitMCTSParameters params;

        params.budgetType = SGA::Budget::FMCALLS;
        params.maxFMCalls = fm;
        params.HEURISTIC = heuristic;
        params.DO_STATE_ABSTRACTION = true;
        params.IS_ACTION_INDEPENDENT = false;
        params.IS_UNGROUPING = true;

		params.K = k_values[currentPoint[0]];

		params.ROLLOUT_LENGTH = rollout_length[currentPoint[1]];

        params.R_THRESHOLD = reward_threshold[currentPoint[2]];

        params.T_THRESHOLD = transition_threshold[currentPoint[3]];

        params.absBatch = 1000;

        
		allAgents[0] = std::make_unique<UnitMCTSAgent>("UGMCTSuAgent", std::move(params));
		return allAgents;
	}
}
