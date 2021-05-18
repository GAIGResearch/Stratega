#include <Evaluators/MCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	MCTSEvaluator::MCTSEvaluator(
		std::vector<float> k_values,		// values of k
		std::vector<int> rollout_length,	// rollout length
		std::vector<int> opponent_script,	// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
		GameConfig& configInput
	)
	: Evaluator("MCTSEvaluator"
	),
		k_values(k_values), rollout_length(rollout_length), opponent_script(opponent_script),
		agents(configInput.generateAgents()), config(configInput), arena(std::make_unique<Arena>(config))
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(k_values.size());
		searchSpaceDims.emplace_back(rollout_length.size());
		searchSpaceDims.emplace_back(opponent_script.size());

		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config.numPlayers = 2;
    }
	
	std::vector<float> MCTSEvaluator::evaluate(std::vector<int> point, int nSamples)
	{
		nSamples = config.levelDefinitions.size();
		float value = 0;

		float agentValue = 0;
		float nrOfWins = 0;
		bool playFirst = false;
		std::cout << "evaluate agent: ";

		currentPoint = point;
		agentValue = arena->runGames([&]() {return generateAgents(); }, 2, 0, 1, config.levelDefinitions.size());

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}

	void MCTSEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "K=" << (k_values[point[0]]) << ", ";
		std::cout << "RL=" << (rollout_length[point[1]]) << ", ";
		std::cout << "OS=" << (rollout_length[point[2]]) << ";";
    }

	std::vector<std::unique_ptr<Agent>> MCTSEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();;

		auto allAgents = config.generateAgents();

		// setup current agent configuration
		MCTSParameters params;
		params.K = k_values[currentPoint[0]];
		params.ROLLOUT_LENGTH = rollout_length[currentPoint[1]];
		switch (currentPoint[2])
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

		allAgents[0] = std::make_unique<MCTSAgent>(std::move(params));
		return allAgents;
	}
}
