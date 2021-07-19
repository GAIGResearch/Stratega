#include <Stratega/Agent/RHEAAgent/RHEAGenome.h>
#include <Stratega/Agent/Agent.h>

namespace SGA {

    RHEAGenome::RHEAGenome(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params)
    {
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);

        size_t length = 0;
        while (!gameState.isGameOver() && actionSpace.size() > 0 && length < params.INDIVIDUAL_LENGTH) {
            // choose and apply random action
            //todo forward random generator to getRandomAction
            auto action = actionSpace.at(rand() % actionSpace.size());
            applyActionToGameState(forwardModel, gameState, action, params);
            actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
            actions.emplace_back(action);
            length++;
        }

        // rate newly created individual
        value = params.getStateHeuristic()->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
    }

    RHEAGenome::RHEAGenome(std::vector<Action>& actions, double value) :
        actions(std::move(actions)), value(value) {}



    void RHEAGenome::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, const Action& action, RHEAParams& params)
    {
        //Roll the game state with our action.
        params.REMAINING_FM_CALLS -= SGA::roll(gameState, forwardModel, action, params.PLAYER_ID, params);

        //Continue rolling the state until the game is over, we run out of budget or this agent can play again. 
        while (!gameState.canPlay(params.PLAYER_ID) && params.REMAINING_FM_CALLS > 0 && !gameState.isGameOver())
        {
            //Roll actions for the opponent(s).
            params.REMAINING_FM_CALLS -= SGA::rollOppOnly(gameState, forwardModel, params);
        }
    }


    void RHEAGenome::mutate(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937& randomGenerator)
    {
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);


        // go through the actions and fill the actionVector of its child
        unsigned long long actIdx = 0;
        while (!gameState.isGameOver() && actionSpace.size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;

            // replace with random portfolio in case of mutate or no portfolio available
            if (mutate || (actIdx < actions.size()))
            {
                auto action = actionSpace.at(rand() % actionSpace.size());
                applyActionToGameState(forwardModel, gameState, action, params);
                actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
                if (actIdx < actions.size())
                {
                    actions[actIdx] = action;
                }
                else
                {
                    actions.emplace_back(action);
                }
            }
            else
            {
                // use previous action or sample a new random one in case the individual is too short
                if (actIdx >= actions.size())
                {
                    actions.emplace_back(actionSpace.at(rand() % actionSpace.size()));
                }
                applyActionToGameState(forwardModel, gameState, actions[actIdx], params);
                actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
            }

            actIdx++;
        }

        // rate mutated individual
        this->value = params.getStateHeuristic()->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
    }


    RHEAGenome RHEAGenome::crossover(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937& randomGenerator, RHEAGenome& parent1, RHEAGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);

        // initialize variables for the new genome to be created
        std::vector<Action> actions;

        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;

        while (!gameState.isGameOver() && actionSpace.size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            // if mutate do a random mutation else apply uniform crossover
            std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;

            // mutation = randomly select a new action for gameStateCopy
            if (mutate)
            {
                auto action = actionSpace.at(rand() % actionSpace.size());
                applyActionToGameState(forwardModel, gameState, action, params);
                actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
                actions.emplace_back(action);
            }
            else
            {
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                RHEAGenome& from = useParent1First ? parent1 : parent2;
                bool validAction = true;
                // check the first parent and choose portfolio if available
                if (actIdx < from.actions.size())
                {
                    validAction = from.actions[actIdx].validate(gameState);
                    if(validAction) 
                        actions.emplace_back(from.actions[actIdx]);
                }
                else
                {
                    // check the second parent and choose portfolio if available
                    from = useParent1First ? parent2 : parent1;
                    if (actIdx < from.actions.size())
                    {
                        validAction = from.actions[actIdx].validate(gameState);
                        if (validAction)
                            actions.emplace_back(from.actions[actIdx]);
                    }
                    else validAction = false;
                }

                if (!validAction)
                {
                    // use a random portfolio by default
                    actions.emplace_back(actionSpace.at(rand() % actionSpace.size()));
                }

                applyActionToGameState(forwardModel, gameState, actions[actIdx], params);
                actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
            }
            actIdx++;
        }

        const double value = params.getStateHeuristic()->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
        return RHEAGenome(actions, value);
    }

    void RHEAGenome::shift(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params)
    {
        // reuse previous solution
        std::rotate(actions.begin(), actions.begin() + 1, actions.end());

        // check if actions are still applicable and if not sample a new one from portfolio
        // always re-sample the last action since it is the rotated action from the previous solution

        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
        for (size_t i = 0; i < actions.size(); i++)
        {
            if (actionSpace.size() == 0)
                break;

            // test if a planned action is still valid. if not, replace with a random one
            // and always replace the last action with a new random one
            // (since the vector has been rotated it does not have any meaning)

            if (i == actions.size() - 1 || !actions[i].validate(gameState))
            {
                actions[i] = actionSpace.at(rand() % actionSpace.size());
            }

            applyActionToGameState(forwardModel, gameState, actions[i], params);
            actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
        }

        // re-evaluate the shifted individual
        value = params.getStateHeuristic()->evaluateGameState(forwardModel, gameState, params.PLAYER_ID);
    }

    void RHEAGenome::toString() const
    {
        std::cout << "RHEAGenome" << "\n";
        std::cout << "\tactions=" << "\n";
        for (const auto& action : actions)
        {
            std::cout << "\t\t" << action.getOwnerID() << ";" << "Type=" << action.getActionType().getName() << "\n";
        }

        std::cout << "\tvalue=" << value << "\n;";
    }

}
