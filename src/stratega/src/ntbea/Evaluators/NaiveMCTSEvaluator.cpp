#include <Stratega/ntbea/Evaluators/NaiveMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
	NaiveMCTSEvaluator::NaiveMCTSEvaluator(
		std::vector<int> rollout_length,
		std::vector<float> epsilon_0,
		std::vector<float> epsilon_local,
		float fm,
        std::string heuristic,
		GameConfig& configInput
	)
	: Evaluator("NaiveMCTSEvaluator"),
		rollout_length(rollout_length),
		epsilon_0(epsilon_0),
		epsilon_local(epsilon_local),
        fm(fm), heuristic(heuristic),
		agents(configInput.generateAgents()), config(configInput)
    {
        arena = std::make_unique<Arena>(config);
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(rollout_length.size());
		searchSpaceDims.emplace_back(epsilon_0.size());
        searchSpaceDims.emplace_back(epsilon_local.size());

		//searchSpaceDims.emplace_back(opponent_script.size());

		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config.numPlayers = 2;
    }


	std::vector<float> NaiveMCTSEvaluator::evaluate(std::vector<int> point, int nSamples)
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


	void NaiveMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "RL=" << (rollout_length[point[0]]) << ", ";
		std::cout << "epsl_g=" << (epsilon_0[point[1]]) << ", ";
        std::cout << "epsl_local=" << (epsilon_local[point[2]]) << ", ";
    }

	std::vector<std::unique_ptr<Agent>> NaiveMCTSEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();

		auto allAgents = config.generateAgents();

		// setup current agent configuration
		NaiveMCTSParameters params;

        params.budgetType = SGA::Budget::FMCALLS;
        params.maxFMCalls = fm;
        params.HEURISTIC = heuristic;
 
		params.rolloutLength = rollout_length[currentPoint[0]];

		params.epsilon_0 = epsilon_0[currentPoint[1]];

        params.epsilon_l = epsilon_local[currentPoint[2]];


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
        
		allAgents[0] = std::make_unique<NaiveMCTSAgent>("NaiveMCTSAgent", std::move(params));
		return allAgents;
	}
}
