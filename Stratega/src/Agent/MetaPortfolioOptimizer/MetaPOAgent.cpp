#include <Agent/MetaPortfolioOptimizer/MetaPOAgent.h>


namespace SGA
{
    void MetaPOAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
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
                if (actionSpace->size() == 1)
                {
                    gameCommunicator.executeAction(actionSpace->at(0));
                    pop_.clear();
                }
            	else
                {
                    auto& rnd = gameCommunicator.getRNGEngine();
            		
            		// either shift previous population or initialize a new population
	                if (params_.CONTINUE_SEARCH && !pop_.empty())
	                {
	                	if (lastKnownTick != gameState.currentGameTurn)
	                	{
                            pop_ = shiftPopulation(forwardModel, gameState, rnd);
                            lastKnownTick = gameState.currentGameTurn;
	                	}
	                }
            		else
	                {
                        initializePopulation(forwardModel, gameState, rnd);
	                }

            		// run rhea and return the best individual of the previous generation
                    rheaLoop(forwardModel, gameState, rnd);

                    gameCommunicator.executeAction(pop_[0].getAction(gameState, actionSpace));

            		/*
					// return all actions until the end of the agent's turn
            		while (params_.PLAYER_ID == gameState.currentPlayer && !gameState.isGameOver)
            		{
                        auto actionToApply = pop_[0].getAction(gameState, actionSpace);
                        gameCommunicator.executeAction(actionToApply);
                        forwardModel.advanceGameState(gameState, actionToApply);
                        actionSpace = forwardModel.getActions(gameState);
            		}
            		*/
                }
			}
        }
    }
   
    void MetaPOAgent::initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
    	// create params_.POP_SIZE new random individuals
        pop_.clear();
        for (size_t i = 0; i < params_.POP_SIZE; i++) {
            pop_.emplace_back(MetaPOGenome(forwardModel, gameState, params_));
        }
    }

    std::vector<MetaPOGenome> MetaPOAgent::shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        std::vector<MetaPOGenome> newPop;

        // we shift the first individual, which is the only one that is likely to be feasible
        std::set<int> unitIDs = std::set<int>();
    	for (TBSUnit* unit : gameState.getPlayer(gameState.currentPlayer)->getUnits())
    	{
            unitIDs.emplace(unit->getUnitID());
    	}
    	
        pop_[0].shift(forwardModel, gameState, params_, unitIDs, randomGenerator);
        newPop.emplace_back(pop_[0]);

        // from 1 to (1+params._MUTATE_BEST), mutate the best individual
        for (size_t i = 1; i <= params_.MUTATE_BEST && i < params_.POP_SIZE; ++i) 
        {
            MetaPOGenome mutGen(pop_[0]);
            mutGen.mutate(forwardModel, gameState, params_, randomGenerator);
            newPop.emplace_back(mutGen);
        }

        // from 1+params.MUTATE_BEST to params_.POP_SIZE, generate at random
        for (size_t i = 1 + params_.MUTATE_BEST; i < params_.POP_SIZE; ++i) {
            newPop.emplace_back(MetaPOGenome(forwardModel, gameState, params_));
        }
        return newPop;
    }
	

    bool sortByFitness(const MetaPOGenome& i, const MetaPOGenome& j) { return i.getValue() > j.getValue(); }
	
    void MetaPOAgent::rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // keep improving the population until the fmCall limit has been reached
        while (params_.REMAINING_FM_CALLS > 0)
        {
            pop_ = nextGeneration(forwardModel, gameState, randomGenerator);
        }
        sort(pop_.begin(), pop_.end(), sortByFitness);
    }

    std::vector<MetaPOGenome> MetaPOAgent::nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        // placeholder for the next generation
        std::vector<MetaPOGenome> newPop;

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
	
    MetaPOGenome MetaPOAgent::getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator)
    {
        if (params_.POP_SIZE > 1)
        {
            std::vector<MetaPOGenome> parents = tournamentSelection();
            return MetaPOGenome::crossover(forwardModel, gameState, params_, randomGenerator, parents[0], parents[1]);
        }
        else {
            MetaPOGenome gMut(pop_[0]);
            gMut.mutate(forwardModel, gameState, params_, randomGenerator);
            return (gMut.getValue() >= pop_[0].getValue()) ? gMut : pop_[0];
        }
    }
	
    std::vector<MetaPOGenome> MetaPOAgent::tournamentSelection()
    {
        std::vector<MetaPOGenome> tournament;
        // sample subset, sort by fitness, select best individual
        // parent 1
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });
    	
        sort(tournament.begin(), tournament.end(), sortByFitness);
        MetaPOGenome& parent1 = tournament[0];

    	
        // parent 2
        tournament.clear();
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });

        std::sort(tournament.begin(), tournament.end(), sortByFitness);
        MetaPOGenome& parent2 = tournament[0];

        return std::vector<MetaPOGenome>{parent1, parent2};
    }
}
