#include <Evaluators/AbstractGameStateMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	AbstractGameStateMCTSEvaluator::AbstractGameStateMCTSEvaluator(
		std::vector<float> k_values,
		std::vector<int> rollout_length,
		std::vector<int> opponent_script_index,
        std::vector<bool> insertMapOptions,
        std::vector<bool> insertPositionsOptions,
		GameConfig& configInput
	)
	: Evaluator("AbstractGameStateMCTSEvaluator"
	),
		k_values(k_values), rollout_length(rollout_length), opponent_script_index(opponent_script_index),
		insertMap(insertMapOptions), insertPositions(insertPositionsOptions), agents(configInput.generateAgents()), config(configInput), arena(std::make_unique<Arena>(config))
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(k_values.size());
		searchSpaceDims.emplace_back(rollout_length.size());
		searchSpaceDims.emplace_back(opponent_script_index.size());
		searchSpaceDims.emplace_back(insertMap.size());
		searchSpaceDims.emplace_back(insertPositions.size());

		// add one dimension per parameter
		auto game = config.generateGameState(0);
		std::map<std::string, bool> insertEntityParameters;
		for (auto entry : *(game->gameInfo->parameterIDLookup)) {
			if (!insertEntityParameters.contains(entry.first))
			{
				insertEntityParameters[entry.first] = false;
				parameterNames.push_back(entry.first);
				searchSpaceDims.emplace_back(2);
			}
		}
    	
		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config.numPlayers = 2;
    }
	
	std::vector<float> AbstractGameStateMCTSEvaluator::evaluate(std::vector<int> point, int nSamples)
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

	void AbstractGameStateMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "K=" << (k_values[point[0]]) << ", ";
		std::cout << "RL=" << (rollout_length[point[1]]) << ", ";
		std::cout << "OS=" << (rollout_length[point[2]]) << ", ";
	    std::cout << "Map=" << (insertMap[point[3]]==1) << ", ";
	    std::cout << "Positions=" << (insertPositions[point[4]] == 1) << ", ";
		for (int i = 5; i < point.size(); i++)
		{
			std::cout << parameterNames[i-5] << "=" << (point[5] == 1) << ", ";
		}
    }

	std::vector<std::unique_ptr<Agent>> AbstractGameStateMCTSEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();;

		auto allAgents = config.generateAgents();
		AbstractMCTSParameters params;

		// setup current agent configuration
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

		// setup current agent configuration
		params.STATE_FACTORY = nullptr;
		StateFactoryConfiguration configuration;
		configuration.insertMap = currentPoint[3] == 1;
		configuration.insertEntityPositions = currentPoint[4] == 1;
		for (int i = 5; i < currentPoint.size(); ++i)
		{
			configuration.insertEntityParameters[parameterNames[i - 5]] = currentPoint[i] == 1;
		}
		params.STATE_FACTORY = std::make_unique<StateFactory>(configuration);
		params.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(*game.get());

		

		allAgents[0] = std::make_unique<AbstractStateMCTSAgent>(std::move(params));
		return allAgents;
	}
}
