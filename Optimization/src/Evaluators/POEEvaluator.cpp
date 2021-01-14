#include <Evaluators/POEEvaluator.h>

#include <Configuration/GameConfig.h>

#include <TBSLogger.h>

namespace SGA
{
    
	POEEvaluator::POEEvaluator(std::vector<int> popSizeCandidates,
        std::vector<int> individualLengthCandidates,
        std::vector<float> mutationRateCandidates,
        std::vector<int> tournamentSize,
        std::vector<bool> elitism,
        std::vector<bool> continueSearch,
		std::vector<bool> p1,
		std::vector<bool> p2,
		std::vector<bool> p3,
		std::vector<bool> p4,
		std::vector<bool> p5,
		std::vector<bool> p6,
		SGA::GameConfig& config
	) : Evaluator("POEEvaluator"
	),
		_popSizeCandidates(popSizeCandidates), _individualLengthCandidates(individualLengthCandidates),
		_mutationRateCandidates(mutationRateCandidates), _tournamentSize(tournamentSize),
		_elitism(elitism), _continueSearch(continueSearch),
		_p1(std::move(p1)), _p2(std::move(p2)), _p3(std::move(p3)),
		_p4(std::move(p4)), _p5(std::move(p5)), _p6(std::move(p6)),
		config(&config), agents(agentsFromConfig(config))
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(popSizeCandidates.size());
		searchSpaceDims.emplace_back(individualLengthCandidates.size());
		searchSpaceDims.emplace_back(mutationRateCandidates.size());
		searchSpaceDims.emplace_back(tournamentSize.size());
		searchSpaceDims.emplace_back(elitism.size());
		searchSpaceDims.emplace_back(continueSearch.size());
		searchSpaceDims.emplace_back(_p1.size());
		searchSpaceDims.emplace_back(_p2.size());
		searchSpaceDims.emplace_back(_p3.size());
		searchSpaceDims.emplace_back(_p4.size());
		searchSpaceDims.emplace_back(_p5.size());
		searchSpaceDims.emplace_back(_p6.size());
    	
		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
		config.numPlayers = 2;
    }
	
    std::vector<float> POEEvaluator::evaluate(std::vector<int> point, int nSamples)
    {
		float value = 0;
    	
		float agentValue = 0;
		int samples = 0;
		bool playFirst = false;
    	
    	while (samples < nSamples)
    	{
			for (int agentID = 0; agentID < agents.size(); agentID++)
			{
				if (samples >= nSamples)
					break;
				agentValue += evaluateGame(point, agentID, playFirst);
				samples++;
			}
			playFirst = !playFirst;
    	}

		
        return { agentValue };
    }

	float POEEvaluator::evaluateGame(std::vector<int> point, int opponentID, bool playFirst)
    {
		auto game = SGA::generateGameFromConfig(*config, rngEngine);
		const std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
		auto agents = agentsFromConfig(*config);

		POEParams params;
		params.POP_SIZE = _popSizeCandidates[point[0]];
		params.INDIVIDUAL_LENGTH = _individualLengthCandidates[point[1]];
		params.MUTATION_RATE = _mutationRateCandidates[point[2]];
		params.TOURNAMENT_SIZE = _tournamentSize[point[3]];
		params.ELITISM = _elitism[point[4]];
		params.CONTINUE_SEARCH = _continueSearch[point[5]];

		std::vector<std::unique_ptr<BaseActionScript>> newPortfolio = std::vector<std::unique_ptr<BaseActionScript>>();	// the PORTFOLIO used to sample actions of a genome
		if (_p1[point[6]])
		{
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			newPortfolio.emplace_back(std::move(attackClose));
		}
		if (_p2[point[7]])
		{
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			newPortfolio.emplace_back(std::move(attackWeak));
		}
		if (_p3[point[8]])
		{
			std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			newPortfolio.emplace_back(std::move(runAway));
		}
		if (_p4[point[9]])
		{
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			newPortfolio.emplace_back(std::move(useSpecialAbility));
		}
		if (_p5[point[10]])
		{
			std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			newPortfolio.emplace_back(std::move(runToFriends));
		}
		if (_p6[point[11]])
		{
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			newPortfolio.emplace_back(std::move(random));
		}

		params.PORTFOLIO = std::move(newPortfolio);
		
		auto agentToEvaluate = std::make_unique<POEAgent>(std::move(params));
    	
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
			return 1;
		return 0;
    }

	void POEEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << _popSizeCandidates[point[0]] << ", ";
	    std::cout << _individualLengthCandidates[point[1]] << ", ";
	    std::cout << _mutationRateCandidates[point[2]] << ", ";
	    std::cout << _tournamentSize[point[3]] << ", ";
	    std::cout << _elitism[point[4]] << ", ";
		std::cout << _continueSearch[point[5]];
		std::cout << _p1[point[6]];
		std::cout << _p2[point[7]];
		std::cout << _p3[point[8]];
		std::cout << _p4[point[9]];
		std::cout << _p5[point[10]];
		std::cout << _p6[point[11]];
    }
    
}
