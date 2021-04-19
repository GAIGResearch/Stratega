#include <Evaluators/RHEAEvaluator.h>

#include <Configuration/GameConfig.h>

#include <TBSLogger.h>

namespace SGA
{
    
	AbstractGameStateMCTSEvaluator::AbstractGameStateMCTSEvaluator(std::vector<int> popSizeCandidates,
        std::vector<int> individualLengthCandidates,
        std::vector<float> mutationRateCandidates,
        std::vector<int> tournamentSize,
        std::vector<bool> elitism,
        std::vector<bool> continueSearch,
		SGA::GameConfig& config
	) : Evaluator("RHEAEvaluator"
	),
		_popSizeCandidates(popSizeCandidates), _individualLengthCandidates(individualLengthCandidates),
		_mutationRateCandidates(mutationRateCandidates), _tournamentSize(tournamentSize),
		_elitism(elitism), _continueSearch(continueSearch), config(&config), agents(agentsFromConfig(config))
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(popSizeCandidates.size());
		searchSpaceDims.emplace_back(individualLengthCandidates.size());
		searchSpaceDims.emplace_back(mutationRateCandidates.size());
		searchSpaceDims.emplace_back(tournamentSize.size());
		searchSpaceDims.emplace_back(elitism.size());
		searchSpaceDims.emplace_back(continueSearch.size());
    	
		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		config.numPlayers = 2;
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
		auto game = SGA::generateGameFromConfig(*config, rngEngine);
		const std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
		auto agents = agentsFromConfig(*config);

		RHEAParams params;
		params.POP_SIZE = _popSizeCandidates[point[0]];
		params.INDIVIDUAL_LENGTH = _individualLengthCandidates[point[1]];
		params.MUTATION_RATE = _mutationRateCandidates[point[2]];
		params.TOURNAMENT_SIZE = _tournamentSize[point[3]];
		params.ELITISM = _elitism[point[4]];
		params.CONTINUE_SEARCH = _continueSearch[point[5]];
    	
		auto agentToEvaluate = std::make_unique<RHEAAgent>(std::move(params));
    	
    	if (playFirst)
    	{
			// set agent to be tested
			auto agentNew = std::move(agentToEvaluate);

			auto agentNewComm = std::make_unique<SGA::TBSGameCommunicator>(0);
			agentNewComm->setAgent(std::move(agentNew));
			agentNewComm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			agentNewComm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(agentNewComm));
    		
			// set opponent
			auto agentOP = std::move(agents[opponentID]);
			if (agentOP == nullptr)
			{
				throw std::runtime_error("Human-agents are not allowed while optimizing parameters.");
			}

			auto OPComm = std::make_unique<SGA::TBSGameCommunicator>(1);
			OPComm->setAgent(std::move(agentOP));
			OPComm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			OPComm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(OPComm));

    	} else
    	{
			// set opponent
			auto agentOP = std::move(agents[opponentID]);
			if (agentOP == nullptr)
			{
				throw std::runtime_error("Human-agents are not allowed while optimizing parameters.");
			}

			auto OPComm = std::make_unique<SGA::TBSGameCommunicator>(0);
			OPComm->setAgent(std::move(agentOP));
			OPComm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			OPComm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(OPComm));

			// set agent to be tested
			auto agentNew = std::move(agentToEvaluate);

			auto agentNewComm = std::make_unique<SGA::TBSGameCommunicator>(1);
			agentNewComm->setAgent(std::move(agentNew));
			agentNewComm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			agentNewComm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(agentNewComm));
    	}
		

		// run game
		//game->addCommunicator(std::make_unique<TBSLogger>(dynamic_cast<SGA::TBSGame&>(*game)));
		game->run();

		// return result
		const int winnerID = dynamic_cast<SGA::TBSGame&>(*game).getState().getWinnerID();
		if ((playFirst && winnerID == 0) || (!playFirst && winnerID == 1))
			return 3;
		if (winnerID == -1)
			return 1;
		return 0;
    }

	void AbstractGameStateMCTSEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << _popSizeCandidates[point[0]] << ", ";
	    std::cout << _individualLengthCandidates[point[1]] << ", ";
	    std::cout << _mutationRateCandidates[point[2]] << ", ";
	    std::cout << _tournamentSize[point[3]] << ", ";
	    std::cout << _elitism[point[4]] << ", ";
		std::cout << _continueSearch[point[5]];
    }
    
}
