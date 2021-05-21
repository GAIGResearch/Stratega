#include <Evaluators/AbstractingEverythingMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	AbstractingEverythingMCTSEvaluator::AbstractingEverythingMCTSEvaluator(
		std::vector<float> k_values,
		std::vector<int> rollout_length,
		std::vector<int> opponent_script_index,
		std::vector<bool> attack_closest,
		std::vector<bool> attack_weakest,
		std::vector<bool> run_away,
		std::vector<bool> run_to_friend,
		std::vector<bool> special_ability,
		std::vector<bool> random_script,
        std::vector<bool> insertMapOptions,
        std::vector<bool> insertPositionsOptions,
		GameConfig& configInput
	)
	: Evaluator("AbstractingEverythingMCTSEvaluator"
	),
		k_values(k_values), rollout_length(rollout_length), opponent_script_index(opponent_script_index),
		attack_closest(attack_closest), attack_weakest(attack_weakest), run_away(run_away),
		run_to_friend(run_to_friend), special_ability(special_ability), random_script(random_script),
		insertMap(insertMapOptions), insertPositions(insertPositionsOptions), agents(configInput.generateAgents()), config(configInput), arena(std::make_unique<Arena>(config))
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(k_values.size());
		searchSpaceDims.emplace_back(rollout_length.size());
		searchSpaceDims.emplace_back(opponent_script_index.size());
		searchSpaceDims.emplace_back(attack_closest.size());
		searchSpaceDims.emplace_back(attack_weakest.size());
		searchSpaceDims.emplace_back(run_away.size());
		searchSpaceDims.emplace_back(run_to_friend.size());
		searchSpaceDims.emplace_back(special_ability.size());
		searchSpaceDims.emplace_back(random_script.size());
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
	
	std::vector<float> AbstractingEverythingMCTSEvaluator::evaluate(std::vector<int> point, int nSamples)
	{
		nSamples = config.levelDefinitions.size();
		float value = 0;

		float agentValue = 0;
		float nrOfWins = 0;
		bool playFirst = false;
		std::cout << "evaluate agent: ";

		currentPoint = point;
		auto results = arena->runGames([&]() {return generateAgents(); }, 2, 0, 1, config.levelDefinitions.size());
		agentValue = results[0];
		nrOfWins = results[1];

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}

	void AbstractingEverythingMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
		std::cout << "K=" << (k_values[point[0]]) << ", ";
		std::cout << "RL=" << (rollout_length[point[1]]) << ", ";
		std::cout << "OS=" << (point[2]) << ", ";
		std::cout << "Scripts={";
		if (point[3]) std::cout << "AC ";
		if (point[4]) std::cout << "AW ";
		if (point[5]) std::cout << "RA ";
		if (point[6]) std::cout << "RF ";
		if (point[7]) std::cout << "SA ";
		if (point[8]) std::cout << "RND ";

	    std::cout << "Map=" << (insertMap[point[9]]==1) << ", ";
	    std::cout << "Positions=" << (insertPositions[point[10]] == 1) << ", ";
		for (int i = 11; i < point.size(); i++)
		{
			std::cout << parameterNames[i-11] << "=" << (point[i] == 1) << ", ";
		}
    }

	std::vector<std::unique_ptr<Agent>> AbstractingEverythingMCTSEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();;

		auto allAgents = config.generateAgents();


		// setup current agent configuration
		AbstractMCTSParameters params;
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


		// the PORTFOLIO used to sample actions of a genome
		std::vector<std::unique_ptr<BaseActionScript>> newPortfolio = std::vector<std::unique_ptr<BaseActionScript>>();
		if (attack_closest[currentPoint[3]])
		{
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			newPortfolio.emplace_back(std::move(attackClose));
		}
		if (attack_weakest[currentPoint[4]])
		{
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			newPortfolio.emplace_back(std::move(attackWeak));
		}
		if (run_away[currentPoint[5]])
		{
			std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			newPortfolio.emplace_back(std::move(runAway));
		}
		if (run_to_friend[currentPoint[6]])
		{
			std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			newPortfolio.emplace_back(std::move(runToFriends));
		}
		if (special_ability[currentPoint[7]])
		{
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			newPortfolio.emplace_back(std::move(useSpecialAbility));
		}
		if (random_script[currentPoint[8]] || newPortfolio.empty())
		{
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			newPortfolio.emplace_back(std::move(random));
		}

		params.PORTFOLIO = std::move(newPortfolio);


		// setup current agent configuration
		params.STATE_FACTORY = nullptr;
		StateFactoryConfiguration configuration;
		configuration.insertMap = currentPoint[9] == 1;
		configuration.insertEntityPositions = currentPoint[10] == 1;
		for (int i = 11; i < currentPoint.size(); ++i)
		{
			configuration.insertEntityParameters[parameterNames[i - 11]] = currentPoint[i] == 1;
		}
		params.STATE_FACTORY = std::make_unique<StateFactory>(configuration);
		params.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(*game.get());

		
		allAgents[0] = std::make_unique<AbstractStateMCTSAgent>(std::move(params));
		return allAgents;
	}
}
