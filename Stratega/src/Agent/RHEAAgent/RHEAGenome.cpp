#include <Agent/RHEAAgent/RHEAGenome.h>

namespace SGA {

    RHEAGenome::RHEAGenome(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params)
    {
        auto actionSpace = forwardModel.getActions(gameState);
        const int playerID = gameState.currentPlayer;

        size_t length = 0;
        while (!gameState.isGameOver && actionSpace->size() > 0 && length < params.INDIVIDUAL_LENGTH) {
            // choose and apply random action
            //todo forward random generator to getRandomAction
            const Action action = actionSpace->at(rand() % actionSpace->size());
            applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
            actions.emplace_back(action);
            length++;
        }

        // rate newly created individual
        value = params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
    }

    RHEAGenome::RHEAGenome(std::vector<Action<Vector2i>>& actions, double value) :
        actions(std::move(actions)), value(value) {}

    void RHEAGenome::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, const Action<Vector2i>& action, RHEAParams& params)
    {
        params.REMAINING_FM_CALLS--;
        forwardModel.advanceGameState(gameState, action);
        while (gameState.currentPlayer != params.PLAYER_ID && !gameState.isGameOver)
        {
            if (params.opponentModel) // use default opponentModel to choose actions until the turn has ended
            {
                params.REMAINING_FM_CALLS--;
                auto opActionSpace = forwardModel.getActions(gameState);
                auto opAction = params.opponentModel->getAction(gameState, opActionSpace);
                forwardModel.advanceGameState(gameState, opAction);
            }
            else // skip opponent turn
            {
                ActionSpace<Vector2i> endTurnActionSpace;
                forwardModel.generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
                forwardModel.advanceGameState(gameState, endTurnActionSpace.at(0));
            }
        }

        actionSpace = forwardModel.getActions(gameState);
    }

    void RHEAGenome::mutate(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params, std::mt19937& randomGenerator)
    {
        auto actionSpace = forwardModel.getActions(gameState);
        const int playerID = gameState.currentPlayer;

        // go through the actions and fill the actionVector of its child
        unsigned long long actIdx = 0;
        while (!gameState.isGameOver && actionSpace->size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;

            // replace with random portfolio in case of mutate or no portfolio available
            if (mutate || (actIdx < actions.size()))
            {
                const Action action = actionSpace->at(rand() % actionSpace->size());
                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
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
                    actions.emplace_back(actionSpace->at(rand() % actionSpace->size()));
                }
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }

            actIdx++;
        }

        // rate mutated individual
        this->value = params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
    }

    RHEAGenome RHEAGenome::crossover(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params, std::mt19937& randomGenerator, RHEAGenome& parent1, RHEAGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.getActions(gameState);
        const int playerID = gameState.currentPlayer;

    	// initialize variables for the new genome to be created
        std::vector<Action<Vector2i>> actions;

        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;
        while (!gameState.isGameOver && actionSpace->size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            // if mutate do a random mutation else apply uniform crossover
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;

            // mutation = randomly select a new action for gameStateCopy
            if (mutate)
            {
                Action<Vector2i> action = actionSpace->at(rand() % actionSpace->size());
                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
                actions.emplace_back(action);
            }
            else
            {
                int chosenPortfolio;
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                RHEAGenome& from = useParent1First ? parent1 : parent2;
                // check the first parent and choose portfolio if available
                if (actIdx < from.actions.size())
                {
                    actions.emplace_back(from.actions[actIdx]);
                }
                else
                {
                    // check the second parent and choose portfolio if available
                    from = useParent1First ? parent2 : parent1;
                    if (actIdx < from.actions.size())
                    {
                        actions.emplace_back(from.actions[actIdx]);
                    }
                    else
                    {
                        // use a random portfolio by default
                        actions.emplace_back(actionSpace->at(rand() % actionSpace->size()));
                    }
                }
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }

            actIdx++;
        }

        const double value = params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
        return RHEAGenome(actions, value);
    }

    void RHEAGenome::shift(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params)
    {
        const int playerID = gameState.currentPlayer;

        // reuse previous solution
        std::rotate(actions.begin(), actions.begin() + 1, actions.end());

        // check if actions are still applicable and if not sample a new one from portfolio
        // always re-sample the last action since it is the rotated action from the previous solution
        auto actionSpace = forwardModel.getActions(gameState);
        for (size_t i = 0; i < actions.size(); i++)
        {
            if (actionSpace->size() == 0)
                break;

            // test if a planned action is still valid. if not, replace with a random one
            // and always replace the last action with a new random one
            // (since the vector has been rotated it does not have any meaning)
            if (i == actions.size() - 1 || !forwardModel.isValid(gameState, actions[i]))
            {
                actions[i] = actionSpace->at(rand() % actionSpace->size());
            }
    	
            applyActionToGameState(forwardModel, gameState, actionSpace, actions[i], params);
        }

        // re-evaluate the shifted individual
        value = params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
    }

    void RHEAGenome::toString() const
    {
        std::cout << "RHEAGenome" << "\n";
        std::cout << "\tactions=" << "\n";
        for (const auto& action : actions)
        {
            std::cout << "\t\t" << action.getPlayerID() << ";" << action.getSourceUnitID() << ";" << action.getTargetUnitID() << "\n";
        }

        std::cout << "\tvalue=" << value << "\n;";
    }
    
}
