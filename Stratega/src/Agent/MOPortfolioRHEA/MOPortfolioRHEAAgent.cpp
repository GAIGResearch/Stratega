#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAAgent.h>


namespace SGA
{
    void MOPortfolioRHEAAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
    {
        while (!gameCommunicator.getGameState().isGameOver)
        {
            if (gameCommunicator.isMyTurn())
            {
                TBSGameState gameState = gameCommunicator.getGameState();
                if (gameState.isGameOver)
                    break;
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
    
    void MOPortfolioRHEAAgent::initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
    	// create params_.POP_SIZE new random individuals
        pop_.clear();
        for (size_t i = 0; i < params_.POP_SIZE; i++) {
            pop_.emplace_back(MOPortfolioRHEAGenome(forwardModel, gameState, params_));
        }
    }

    std::vector<MOPortfolioRHEAGenome> MOPortfolioRHEAAgent::shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        std::vector<MOPortfolioRHEAGenome> newPop;

        // we shift the first individual, which is the only one that is likely to be feasible
        pop_[0].shift(forwardModel, gameState, params_);
        newPop.emplace_back(pop_[0]);

        // from 1 to (1+params._MUTATE_BEST), mutate the best individual
        for (size_t i = 1; i <= params_.MUTATE_BEST && i < params_.POP_SIZE; ++i) 
        {
            MOPortfolioRHEAGenome mutGen(pop_[0]);
            mutGen.mutate(forwardModel, gameState, params_, randomGenerator);
            newPop.emplace_back(mutGen);
        }

        // from 1+params.MUTATE_BEST to params_.POP_SIZE, generate at random
        for (size_t i = 1 + params_.MUTATE_BEST; i < params_.POP_SIZE; ++i) {
            newPop.emplace_back(MOPortfolioRHEAGenome(forwardModel, gameState, params_));
        }
        return newPop;
    }
	

    bool sortByFitness(const MOPortfolioRHEAGenome& i, const MOPortfolioRHEAGenome& j)
    {    	
        bool flag = (i.value1 < j.value1 && i.value2 <= j.value2) ||
            (i.value2 < j.value2 && i.value1 <= j.value1);
        if (flag) // i dominates j
            return true;

    	flag = (j.value1 < i.value1&& j.value2 <= i.value2) ||
            (j.value2 < i.value2&& j.value1 <= i.value1);
        if (flag) // ind2 dominates ind1
            return false;
    	
        if (i.crowdingDistance > j.crowdingDistance)
            return true;
    	
        if (j.crowdingDistance > i.crowdingDistance)
            return false;
    	
	    return false;
    }
	
    void MOPortfolioRHEAAgent::rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // keep improving the population until the fmCall limit has been reached
        while (params_.REMAINING_FM_CALLS > 0)
        {
            calculateCrowdingDistances(pop_);
            pop_ = nextGeneration(forwardModel, gameState, randomGenerator);
        }
        sort(pop_.begin(), pop_.end(), sortByFitness);
    }

    std::vector<MOPortfolioRHEAGenome> MOPortfolioRHEAAgent::nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // placeholder for the next generation
        std::vector<MOPortfolioRHEAGenome> newPop;

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

    	// 
        return newPop;
    }
	
    MOPortfolioRHEAGenome MOPortfolioRHEAAgent::getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        if (params_.POP_SIZE > 1)
        {
            std::vector<MOPortfolioRHEAGenome> parents = tournamentSelection();
            return MOPortfolioRHEAGenome::crossover(forwardModel, gameState, params_, randomGenerator, parents[0], parents[1]);
        }
        else {
            MOPortfolioRHEAGenome gMut(pop_[0]);
            gMut.mutate(forwardModel, gameState, params_, randomGenerator);
            return (gMut.value1 < pop_[0].value1 && gMut.value2 <= pop_[0].value2) || 
                (gMut.value2 < pop_[0].value2 && gMut.value1 <= pop_[0].value1) ? gMut : pop_[0];
        }
    }
	
    std::vector<MOPortfolioRHEAGenome> MOPortfolioRHEAAgent::tournamentSelection()
    {
        std::vector<MOPortfolioRHEAGenome> tournament;
        // sample subset, sort by fitness, select best individual
        // parent 1
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });
        sort(tournament.begin(), tournament.end(), sortByFitness);
        MOPortfolioRHEAGenome& parent1 = tournament[0];

    	
        // parent 2
        tournament.clear();
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });

        std::sort(tournament.begin(), tournament.end(), sortByFitness);
        MOPortfolioRHEAGenome& parent2 = tournament[0];

        return std::vector<MOPortfolioRHEAGenome>{parent1, parent2};
    }

    
    void MOPortfolioRHEAAgent::calculateCrowdingDistances(std::vector<MOPortfolioRHEAGenome>& population)
    {
    	
        std::vector<std::vector<MOPortfolioRHEAGenome*>> fronts = getFronts(population);
        //std::vector<std::vector<MOPortfolioRHEAGenome>> fronts = std::vector<std::vector<MOPortfolioRHEAGenome&>>();
    	
        for (std::vector<MOPortfolioRHEAGenome*>& front : fronts)
        {
            if (!front.empty())
                front[0]->crowdingDistance = std::numeric_limits<double>::max();
            if (front.size() > 1)
                front[front.size() - 1]->crowdingDistance = std::numeric_limits<double>::max();

            for (int i = 1; i < front.size() - 1; i++)
            {
                front[i]->crowdingDistance = abs(front[i - 1]->value1 - front[i + 1]->value1) +
                    abs(front[i - 1]->value2 - front[i + 1]->value2);
            }
        }
    }

    
    bool sortByFitnessValue1(const MOPortfolioRHEAGenome* i, const MOPortfolioRHEAGenome* j)
    {
        return i->value1 < j->value1;
    }

	
    std::vector<std::vector<MOPortfolioRHEAGenome*>> MOPortfolioRHEAAgent::getFronts(std::vector<MOPortfolioRHEAGenome>& population)
    {
        auto fronts = std::vector<std::vector<MOPortfolioRHEAGenome*>>();
    	
        std::vector<MOPortfolioRHEAGenome*> unsorted;
        for (MOPortfolioRHEAGenome& genome : population)
        {
            unsorted.emplace_back(&genome);
        }
        
    	std::sort(unsorted.begin(), unsorted.end(), sortByFitnessValue1);

    	while (!unsorted.empty())
    	{
            for (MOPortfolioRHEAGenome* genome : unsorted)
            {
                genome->dominated = false;
            }

            
    		for (MOPortfolioRHEAGenome* genome : unsorted)
    		{
                if (genome->dominated)
                    continue;
    			
                for (MOPortfolioRHEAGenome* genome2 : unsorted)
                {
                    if ((genome->value1 < genome2->value1 && genome->value2 <= genome2->value2) ||
                        (genome->value2 < genome2->value2 && genome->value1 <= genome2->value1))
                    {
                        genome2->dominated = true;
                    }
                }
    		}
            
            std::vector<MOPortfolioRHEAGenome*> front;
            std::vector<MOPortfolioRHEAGenome*> new_unsorted = std::vector<MOPortfolioRHEAGenome*>();
            for (MOPortfolioRHEAGenome* genome : unsorted)
            {
                if (genome->dominated == false)
                    front.emplace_back(genome);
                else
                    new_unsorted.emplace_back(genome);
            }
            fronts.emplace_back(front);
            unsorted = new_unsorted;
            
    	}
    	
        return fronts;
    }
    
	
}
