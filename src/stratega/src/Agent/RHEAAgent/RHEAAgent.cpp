#include <Stratega/Agent/RHEAAgent/RHEAAgent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <algorithm>

namespace SGA
{
    //Initializes the agent. Sets player ID, heuristic and opponent model.
    void RHEAAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
    {
        params_.PLAYER_ID = getPlayerID();
        if (params_.heuristic == nullptr)
            params_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        if (params_.budgetType == Budget::UNDEFINED)
            params_.budgetType = Budget::TIME;
        params_.opponentModel = std::make_shared<RandomActionScript>();
    }

    ActionAssignment RHEAAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        //Initialize the budget for this action call.
        params_.resetCounters(timer);
        //params_.printDetails();

        //Action space from this state, for this plaer.
        auto actionSpace = forwardModel.generateActions(state, getPlayerID());

        // in case only one action is available the player turn ends
        // throw away previous solutions because we don't know what our opponent will do
        if (actionSpace.size() == 1)
        {
            pop_.clear();
            return ActionAssignment::fromSingleAction(actionSpace.at(0));
        }
        else
        {
            // either shift previous population or initialize a new population
            if (params_.continuePreviousSearch && !pop_.empty())
            {
                pop_ = shiftPopulation(forwardModel, state, getRNGEngine());
            }
            else
            {
                initializePopulation(forwardModel, state, getRNGEngine());
            }

            // run rhea and return the best individual of the last generation
            rheaLoop(forwardModel, state, getRNGEngine());
            return ActionAssignment::fromSingleAction(pop_[0].getActions().front());
        }
    }

    void RHEAAgent::initializePopulation(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937&)
    {
        // create params_.popSize new random individuals
        pop_.clear();
        for (size_t i = 0; i < params_.popSize; i++) {
            pop_.emplace_back(RHEAGenome(forwardModel, gameState, params_));
        }
        params_.currentIterations += static_cast<int>(params_.popSize);
    }


    std::vector<RHEAGenome> RHEAAgent::shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator)
    {
        std::vector<RHEAGenome> newPop;

        // we shift the first individual, which is the only one that is likely to be feasible
        pop_[0].shift(forwardModel, gameState, params_);
        newPop.emplace_back(pop_[0]);

        // from 1 to (1+params._MUTATE_BEST), mutate the best individual
        for (size_t i = 1; i <= params_.mutateBestN && i < params_.popSize; ++i)
        {
            RHEAGenome mutGen(pop_[0]);
            mutGen.mutate(forwardModel, gameState, params_, randomGenerator);
            newPop.emplace_back(mutGen);
        }

        // from 1+params.mutateBestN to params_.popSize, generate at random
        for (size_t i = 1 + params_.mutateBestN; i < params_.popSize; ++i) {
            newPop.emplace_back(RHEAGenome(forwardModel, gameState, params_));
        }


        //In RHEA, we count iterations as individual evaluations.
        params_.currentIterations += static_cast<int>(params_.popSize);

        return newPop;
    }


    bool sortByFitness(const RHEAGenome& i, const RHEAGenome& j) { return i.getValue() > j.getValue(); }

    void RHEAAgent::rheaLoop(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator)
    {
        // keep improving the population until the fmCall limit has been reached
        while (!params_.isBudgetOver() && !gameState.isGameOver())
        {
            pop_ = nextGeneration(forwardModel, gameState, randomGenerator);
        }
        sort(pop_.begin(), pop_.end(), sortByFitness);
    }

    std::vector<RHEAGenome> RHEAAgent::nextGeneration(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator)
    {
        // placeholder for the next generation
        std::vector<RHEAGenome> newPop;

        // sort the population and add the best to the next generation (elitism)
        sort(pop_.begin(), pop_.end(), sortByFitness);
        if (params_.elitism && params_.popSize > 1)
        {
            newPop.emplace_back(pop_[0]);
        }

        // add further individuals until the generation is full
        while (newPop.size() < params_.popSize) {
            newPop.emplace_back(getNextGenerationIndividual(forwardModel, gameState, randomGenerator));
        }

        return newPop;
    }

    RHEAGenome RHEAAgent::getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator)
    {
        params_.currentIterations++;
        if (params_.popSize > 1)
        {
            std::vector<RHEAGenome> parents = tournamentSelection();
            return RHEAGenome::crossover(forwardModel, gameState, params_, randomGenerator, parents[0], parents[1]);
        }
        else {
            RHEAGenome gMut(pop_[0]);
            gMut.mutate(forwardModel, gameState, params_, randomGenerator);
            return (gMut.getValue() >= pop_[0].getValue()) ? gMut : pop_[0];
        }
    }

    std::vector<RHEAGenome> RHEAAgent::tournamentSelection()
    {
        std::vector<RHEAGenome> tournament;
       // // sample subset, sort by fitness, select best individual
       // // parent 1
       ///* std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
       //     params_.tournamentSize, boost::mt19937{ std::random_device{}() });*/
       // /*auto parent1 = pop_;
       // std::shuffle(parent1.begin(), parent1.end(), boost::mt19937{ std::random_device{}() });*/

       // sort(tournament.begin(), tournament.end(), sortByFitness);
       // RHEAGenome& parent1 = tournament[0];


       // // parent 2
       ///* tournament.clear();
       // std::sample(pop_.begin(), pop_.end(), std::back_inserter(tournament),
       //     params_.tournamentSize, boost::mt19937{ std::random_device{}() });

       // std::sort(tournament.begin(), tournament.end(), sortByFitness);
       // RHEAGenome& parent2 = tournament[0];

       // return std::vector<RHEAGenome>{parent1, parent2};*/
        return tournament;
    }

}
