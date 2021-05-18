#include <Evaluators/AbstractingEverythingMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	AbstractingEverythingMCTSEvaluator::AbstractingEverythingMCTSEvaluator(
        std::vector<bool> insertMapOptions,
        std::vector<bool> insertPositionsOptions,
		GameConfig& configInput
	)
	: Evaluator("AbstractingEverythingMCTSEvaluator"
	),
		insertMap(insertMapOptions), insertPositions(insertPositionsOptions), agents(configInput.generateAgents()), config(configInput), arena(std::make_unique<Arena>(config))
    {
		std::vector<int> searchSpaceDims;
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
		agentValue = arena->runGames([&]() {return generateAgents(); }, 2, 0, 1, config.levelDefinitions.size());

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}

	void AbstractingEverythingMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << "Map=" << (insertMap[point[0]]==1) << ", ";
	    std::cout << "Positions=" << (insertPositions[point[1]] == 1) << ", ";
		for (int i = 2; i < point.size(); i++)
		{
			std::cout << parameterNames[i-2] << "=" << (point[i] == 1) << ", ";
		}
    }

	std::vector<std::unique_ptr<Agent>> AbstractingEverythingMCTSEvaluator::generateAgents() {
		std::unique_ptr<GameState> game = config.generateGameState();;

		auto allAgents = config.generateAgents();

		// setup current agent configuration
		AbstractMCTSParameters params;
		params.STATE_FACTORY = nullptr;
		StateFactoryConfiguration configuration;
		configuration.insertMap = currentPoint[0] == 1;
		configuration.insertEntityPositions = currentPoint[1] == 1;
		for (int i = 2; i < currentPoint.size(); ++i)
		{
			configuration.insertEntityParameters[parameterNames[i - 2]] = currentPoint[i] == 1;
		}
		params.STATE_FACTORY = std::make_unique<StateFactory>(configuration);
		params.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(*game.get());

		

		allAgents[0] = std::make_unique<AbstractStateMCTSAgent>(std::move(params));
		return allAgents;
	}
}
