#include <Evaluators/AbstractGameStateMCTSEvaluator.h>

#include <Stratega/Configuration/GameConfig.h>

#include <TBSLogger.h>

namespace SGA
{
    
	AbstractGameStateMCTSEvaluator::AbstractGameStateMCTSEvaluator(
        std::vector<bool> insertMapOptions,
        std::vector<bool> insertPositionsOptions,
		GameConfig* config
	)
	: Evaluator("AbstractGameStateMCTSEvaluator"
	),
		insertMap(insertMapOptions), insertPositions(insertPositionsOptions), config(config), agents(config->generateAgents())
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(insertMap.size());
		searchSpaceDims.emplace_back(insertPositions.size());

		// add one dimension per parameter
		auto game = generateAbstractGameFromConfig(*config, rngEngine);
		std::map<std::string, bool> insertEntityParameters;
		for (auto entry : *((TBSGame*)game.get())->getState().gameInfo->parameterIDLookup) {
			if (!insertEntityParameters.contains(entry.first))
			{
				insertEntityParameters[entry.first] = false;
				parameterNames.push_back(entry.first);
				searchSpaceDims.emplace_back(2);
			}
		}
		game->close();

    	
		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		std::cout << "search space size = " << _searchSpace->getSize() << std::endl;
		config->numPlayers = 2;
    }
	
	std::vector<float> AbstractGameStateMCTSEvaluator::evaluate(std::vector<int> point, int nSamples)
	{
		float value = 0;

		float agentValue = 0;
		float nrOfWins = 0;
		int samples = 0;
		bool playFirst = false;
		std::cout << "evaluate agent " << nSamples << " times: ";

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
		}

		std::cout << std::endl;

		return { agentValue, nrOfWins };
	}

	float AbstractGameStateMCTSEvaluator::evaluateGame(std::vector<int> point, int opponentID, bool playFirst)
    {
		auto game = generateAbstractGameFromConfig(*config, rngEngine);;
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
		const int winnerID = dynamic_cast<SGA::TBSGame&>(*game).getState().winnerPlayerID;
		if ((playFirst && winnerID == 0) || (!playFirst && winnerID == 1))
			return 3;
		if (winnerID == -1)
			return 1;
		return 0;
    }

	void AbstractGameStateMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << "Map=" << (insertMap[point[0]]==1) << ", ";
	    std::cout << "Positions=" << (insertPositions[point[1]] == 1) << ", ";
		for (int i = 2; i < point.size(); i++)
		{
			std::cout << parameterNames[i] << "=" << (point[i] == 1) << ", ";
		}
    }
    
}
