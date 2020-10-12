#include <Agent/PortfolioRHEA/PortfolioRHEAAgent.h>


namespace SGA
{
    void PortfolioRHEAAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
    {
        while (!gameCommunicator.getGameState().isGameOver)
        {
            if (gameCommunicator.isMyTurn())
            {
                TBSGameState gameState = gameCommunicator.getGameState();
                auto actionSpace = forwardModel.getActions(gameState);

                params_.REMAINING_FM_CALLS = params_.MAX_FM_CALLS;  // reset number of available forward model calls
                params_.PLAYER_ID = gameState.currentPlayer;        // todo move into agent initialization

            	// in case only one action is available the player turn ends
            	// throw away previous solutions because we don't know what our opponent will do
                if (actionSpace->count() == 1)
                {
                    gameCommunicator.executeAction(actionSpace->getAction(0));
                    pop_.clear();
                }
            	else
                {
                    auto& rnd = gameCommunicator.getRNGEngine();
            		
            		// either shift previous population or initialize a new population
	                if (params_.CONTINUE_SEARCH && !pop_.empty())
	                {
                        pop_ = shiftPopulation(forwardModel, gameState, rnd);
	                }
            		else
	                {
                        initializePopulation(forwardModel, gameState, rnd);
	                }

            		// run rhea and return the best individual of the previous generation
                    rheaLoop(forwardModel, gameState, rnd);
                    gameCommunicator.executeAction(pop_[0].getActions().front());
                }
			}
        }
    }
   
    void PortfolioRHEAAgent::initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
    	// create params_.POP_SIZE new random individuals
        pop_.clear();
        for (size_t i = 0; i < params_.POP_SIZE; i++) {
            pop_.emplace_back(PortfolioRHEAGenome(forwardModel, gameState, params_));
        }
    }

    std::vector<PortfolioRHEAGenome> PortfolioRHEAAgent::shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        std::vector<PortfolioRHEAGenome> newPop;

        // we shift the first individual, which is the only one that is likely to be feasible
        pop_[0].shift(forwardModel, gameState, params_);
        newPop.emplace_back(pop_[0]);

        // from 1 to (1+params._MUTATE_BEST), mutate the best individual
        for (size_t i = 1; i <= params_.MUTATE_BEST && i < params_.POP_SIZE; ++i) 
        {
            PortfolioRHEAGenome mutGen(pop_[0]);
            mutGen.mutate(forwardModel, gameState, params_, randomGenerator);
            newPop.emplace_back(mutGen);
        }

        // from 1+params.MUTATE_BEST to params_.POP_SIZE, generate at random
        for (size_t i = 1 + params_.MUTATE_BEST; i < params_.POP_SIZE; ++i) {
            newPop.emplace_back(PortfolioRHEAGenome(forwardModel, gameState, params_));
        }
        return newPop;
    }
	

    bool sortByFitness(const PortfolioRHEAGenome& i, const PortfolioRHEAGenome& j) { return i.getValue() > j.getValue(); }
	
    void PortfolioRHEAAgent::rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // keep improving the population until the fmCall limit has been reached
        while (params_.REMAINING_FM_CALLS > 0)
        {
            pop_ = nextGeneration(forwardModel, gameState, randomGenerator);
        }
        sort(pop_.begin(), pop_.end(), sortByFitness);
    }

    std::vector<PortfolioRHEAGenome> PortfolioRHEAAgent::nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // placeholder for the next generation
        std::vector<PortfolioRHEAGenome> newPop;

        // sort the population and add the best to the next generation (ELITISM)
        sort(pop_.begin(), pop_.end(), sortByFitness);
        if (params_.ELITISM && params_.POP_SIZE > 1)
        {
            newPop.emplace_back(pop_[0]);
        }

        // add further individuals until the generation is full
        while (newPop.size() < params_.POP_SIZE) {
            newPop.emplace_back(getNextGenerationIndividual(forwardModel, gameState, randomGenerator));
        }

        return newPop;
    }
	
    PortfolioRHEAGenome PortfolioRHEAAgent::getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        if (params_.POP_SIZE > 1)
        {
            std::vector<PortfolioRHEAGenome> parents = tournamentSelection();
            return PortfolioRHEAGenome::crossover(forwardModel, gameState, params_, randomGenerator, parents[0], parents[1]);
        }
        else {
            PortfolioRHEAGenome gMut(pop_[0]);
            gMut.mutate(forwardModel, gameState, params_, randomGenerator);
            return (gMut.getValue() >= pop_[0].getValue()) ? gMut : pop_[0];
        }
    }
	
    std::vector<PortfolioRHEAGenome> PortfolioRHEAAgent::tournamentSelection()
    {
        std::vector<PortfolioRHEAGenome> tournament;
        // sample subset, sort by fitness, select best individual
        // parent 1
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });
    	
        sort(tournament.begin(), tournament.end(), sortByFitness);
        PortfolioRHEAGenome& parent1 = tournament[0];

    	
        // parent 2
        tournament.clear();
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });

        std::sort(tournament.begin(), tournament.end(), sortByFitness);
        PortfolioRHEAGenome& parent2 = tournament[0];

        return std::vector<PortfolioRHEAGenome>{parent1, parent2};
    }
}
