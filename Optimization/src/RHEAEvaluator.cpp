#include <RHEAEvaluator.h>

#include <Configuration/GameConfig.h>

#include "../../Arena/include/TBSLogger.h"
#include <Agent/RHEAAgent/RHEAAgent.h>

namespace SGA
{
    
    RHEAEvaluator::RHEAEvaluator(std::vector<int> popSizeCandidates,
        std::vector<int> individualLengthCandidates,
        std::vector<float> mutationRateCandidates,
        std::vector<int> tournamentSize,
        std::vector<bool> elitism,
        std::vector<bool> continueSearch,
		SGA::GameConfig& config
	) : Evaluator("RHEAEvaluator",
		nullptr
	),
		_popSizeCandidates(popSizeCandidates), _individualLengthCandidates(individualLengthCandidates),
		_mutationRateCandidates(mutationRateCandidates), _tournamentSize(tournamentSize),
		_elitism(elitism), _continueSearch(continueSearch), config(&config)
    {
		std::vector<int> searchSpaceDims;
		searchSpaceDims.emplace_back(popSizeCandidates.size());
		searchSpaceDims.emplace_back(individualLengthCandidates.size());
		searchSpaceDims.emplace_back(mutationRateCandidates.size());
		searchSpaceDims.emplace_back(tournamentSize.size());
		searchSpaceDims.emplace_back(elitism.size());
		searchSpaceDims.emplace_back(continueSearch.size());
    	
		_searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
    }
	
    std::vector<float> RHEAEvaluator::evaluate(std::vector<int> point, int nSamples)
    {
		float value = 0;

    	/*
		// todo: evaluate the agent
		value += _popSizeCandidates[point[0]];
		value += _individualLengthCandidates[point[1]];
		value += _mutationRateCandidates[point[2]];
		value += _tournamentSize[point[3]];
		value += _elitism[point[4]];
		value += _continueSearch[point[5]];

		const std::vector<int> agentAssignment({0, 1});
    	*/
    	
		std::cout << "Initializing new game" << std::endl;
		std::mt19937 rngEngine(0);
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

    	for (int agentID = 0; agentID < agents.size(); agentID++)
    	{
			// set opponent
			auto agentOP = std::move(agents[agentID]);
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

			auto agentNewComm = std::make_unique<SGA::TBSGameCommunicator>(0);
			agentNewComm->setAgent(std::move(agentNew));
			agentNewComm->setGame(dynamic_cast<SGA::TBSGame&>(*game));
			agentNewComm->setRNGEngine(std::mt19937(distribution(rngEngine)));
			game->addCommunicator(std::move(agentNewComm));

			// run game
			game->addCommunicator(std::make_unique<TBSLogger>(dynamic_cast<SGA::TBSGame&>(*game)));
			game->run();
			int winnerID = dynamic_cast<SGA::TBSGame&>(*game).getState().getWinnerID();
			std::cout << "winner: " << winnerID << std::endl;
    	}
		
        return {value};
    }

	void RHEAEvaluator::printPoint(const std::vector<int>& point)
    {
	    std::cout << _popSizeCandidates[point[0]] << ", ";
	    std::cout << _individualLengthCandidates[point[1]] << ", ";
	    std::cout << _mutationRateCandidates[point[2]] << ", ";
	    std::cout << _tournamentSize[point[3]] << ", ";
	    std::cout << _elitism[point[4]] << ", ";
		std::cout << _continueSearch[point[5]];
    }
    
}
