#include <Evaluators/AbstractGameStateMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>


namespace SGA
{
    
	AbstractGameStateMCTSEvaluator::AbstractGameStateMCTSEvaluator(
        std::vector<bool> insertMapOptions,
        std::vector<bool> insertPositionsOptions,
		GameConfig& configInput
	)
	: Evaluator("AbstractGameStateMCTSEvaluator"
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

		/*
		while (samples < nSamples)
		{
			for (int agentID = 0; agentID < agents.size(); agentID++)
			{
				if (samples >= nSamples)
					break;
				int newValue = evaluateGame(point, agentID, playFirst);
				agentValue += newValue;
				if (newValue == 3)
					nrOfWins += 1;
				samples++;
				std::cout << "x";
				if (samples % 5 == 0)
					std::cout << " ";
			}
			playFirst = !playFirst;
		}*/

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}

	/*
	float AbstractGameStateMCTSEvaluator::evaluateGame(std::vector<int> point, int opponentID, bool playFirst)
    {
		auto game = config.generateGameState();;
		const std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
		auto agents = config->generateAgents();


		// setup current agent configuration
		AbstractMCTSParameters params;
		params.STATE_FACTORY = nullptr;
		StateFactoryConfiguration configuration;
		configuration.insertMap = point[0] == 1;
		configuration.insertEntityPositions = point[1] == 1;
		for (int i = 2; i < point.size(); ++i)
		{
			configuration.insertEntityParameters[parameterNames[i-2]] = point[i] == 1;
		}
		params.STATE_FACTORY = std::make_unique<StateFactory>(configuration);
		GameState state = ((TBSGame*)game.get())->getState();
		params.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(state);


		// create agent to be tested
		auto agentToEvaluate = std::make_unique<AbstractStateMCTSAgent>(std::move(params));
		
    	if (playFirst)
    	{
			// set agent to be tested
			auto agentNew = std::move(agentToEvaluate);
			
			auto agentNewComm = std::make_unique<SGA::AgentGameCommunicator>(0, dynamic_cast<SGA::TBSGame&>(*game), std::move(agentNew), std::mt19937(distribution(rngEngine)));

			// set opponent
			auto agentOP = std::move(agents[opponentID]);
			if (agentOP == nullptr)
			{
				throw std::runtime_error("Human-agents are not allowed while optimizing parameters.");
			}
			auto OPComm = std::make_unique<SGA::AgentGameCommunicator>(1, dynamic_cast<SGA::TBSGame&>(*game), std::move(agentOP), std::mt19937(distribution(rngEngine)));


    	} else
    	{
			// set opponent
			auto agentOP = std::move(agents[opponentID]);
			if (agentOP == nullptr)
			{
				throw std::runtime_error("Human-agents are not allowed while optimizing parameters.");
			}
			auto OPComm = std::make_unique<SGA::AgentGameCommunicator>(0, dynamic_cast<SGA::TBSGame&>(*game), std::move(agentOP), std::mt19937(distribution(rngEngine)));


			// set agent to be tested
			auto agentNew = std::move(agentToEvaluate);
			auto agentNewComm = std::make_unique<SGA::AgentGameCommunicator>(1, dynamic_cast<SGA::TBSGame&>(*game), std::move(agentNew), std::mt19937(distribution(rngEngine)));
    	}
		

		// run game
		//game->addCommunicator(std::make_unique<TBSLogger>(dynamic_cast<SGA::TBSGame&>(*game)));
		game->run();

		// return result
		const int winnerID = dynamic_cast<SGA::GameState&>(*game).getState().winnerPlayerID;
		if ((playFirst && winnerID == 0) || (!playFirst && winnerID == 1))
			return 3;
		if (winnerID == -1)
			return 1;
		return 0;
    }
	*/

	void AbstractGameStateMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << "Map=" << (insertMap[point[0]]==1) << ", ";
	    std::cout << "Positions=" << (insertPositions[point[1]] == 1) << ", ";
		for (int i = 2; i < point.size(); i++)
		{
			std::cout << parameterNames[i-2] << "=" << (point[i] == 1) << ", ";
		}
    }

	std::vector<std::unique_ptr<Agent>> AbstractGameStateMCTSEvaluator::generateAgents() {
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
