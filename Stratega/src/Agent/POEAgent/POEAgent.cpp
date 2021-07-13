#include <Stratega/Agent/POEAgent/POEAgent.h>


namespace SGA
{
    ActionAssignment POEAgent::computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs)
    {
        auto actionSpace = forwardModel->generateActions(state, params_.PLAYER_ID);

        params_.REMAINING_FM_CALLS = params_.MAX_FM_CALLS;  // reset number of available forward model calls

        // in case only one action is available the player turn ends
        // throw away previous solutions because we don't know what our opponent will do
        if (actionSpace.size() == 1)
        {
            pop_.clear();
            return ActionAssignment::fromSingleAction(actionSpace.at(0));
        }
        else
        {
            auto& rnd = getRNGEngine();

            // either shift previous population or initialize a new population
            if (params_.CONTINUE_SEARCH && !pop_.empty())
            {
                pop_ = shiftPopulation(*forwardModel, state, rnd);
            }
            else
            {
                initializePopulation(*forwardModel, state, rnd);
            }

            // run rhea and return the best individual of the previous generation
            rheaLoop(*forwardModel, state, rnd);
        	
            return ActionAssignment::fromSingleAction(pop_[0].getAction(state, actionSpace));
        }
    }

    void POEAgent::init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs)
    {
        params_.PLAYER_ID = getPlayerID();
        if (params_.heuristic == nullptr)
        {
            params_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        }

    }
	
    void POEAgent::initializePopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator)
    {
        // create params_.POP_SIZE new random individuals
        pop_.clear();
        for (size_t i = 0; i < params_.POP_SIZE; i++) {
            pop_.emplace_back(POEGenome(forwardModel, gameState, params_));
        }
    }

    std::vector<POEGenome> POEAgent::shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator)
    {
        std::vector<POEGenome> newPop;

        // we shift the first individual, which is the only one that is likely to be feasible
        pop_[0].shift(forwardModel, gameState, params_);
        newPop.emplace_back(pop_[0]);

        // from 1 to (1+params._MUTATE_BEST), mutate the best individual
        for (size_t i = 1; i <= params_.MUTATE_BEST && i < params_.POP_SIZE; ++i)
        {
            POEGenome mutGen(pop_[0]);
            mutGen.mutate(forwardModel, gameState, params_, randomGenerator);
            newPop.emplace_back(mutGen);
        }

        // from 1+params.MUTATE_BEST to params_.POP_SIZE, generate at random
        for (size_t i = 1 + params_.MUTATE_BEST; i < params_.POP_SIZE; ++i) {
            newPop.emplace_back(POEGenome(forwardModel, gameState, params_));
        }
        return newPop;
    }


    bool sortByFitness(const POEGenome& i, const POEGenome& j) { return i.getValue() > j.getValue(); }

    void POEAgent::rheaLoop(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator)
    {
        // keep improving the population until the fmCall limit has been reached
        while (params_.REMAINING_FM_CALLS > 0)
        {
            pop_ = nextGeneration(forwardModel, gameState, randomGenerator);
        }
        sort(pop_.begin(), pop_.end(), sortByFitness);
    }

    std::vector<POEGenome> POEAgent::nextGeneration(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator)
    {
        // placeholder for the next generation
        std::vector<POEGenome> newPop;

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

    POEGenome POEAgent::getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator)
    {
        if (params_.POP_SIZE > 1)
        {
            std::vector<POEGenome> parents = tournamentSelection();
            return POEGenome::crossover(forwardModel, gameState, params_, randomGenerator, parents[0], parents[1]);
        }
        else {
            POEGenome gMut(pop_[0]);
            gMut.mutate(forwardModel, gameState, params_, randomGenerator);
            return (gMut.getValue() >= pop_[0].getValue()) ? gMut : pop_[0];
        }
    }

    std::vector<POEGenome> POEAgent::tournamentSelection()
    {
        std::vector<POEGenome> tournament;
        // sample subset, sort by fitness, select best individual
        // parent 1
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });

        sort(tournament.begin(), tournament.end(), sortByFitness);
        POEGenome& parent1 = tournament[0];


        // parent 2
        tournament.clear();
        std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
            params_.TOURNAMENT_SIZE, std::mt19937{ std::random_device{}() });

        std::sort(tournament.begin(), tournament.end(), sortByFitness);
        POEGenome& parent2 = tournament[0];

        return std::vector<POEGenome>{parent1, parent2};
    }
}
