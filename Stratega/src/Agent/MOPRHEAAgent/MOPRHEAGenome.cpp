#include <Stratega/Agent/MOPRHEAAgent/MOPRHEAGenome.h>

#include <Stratega/Agent/PRHEAAgent/PRHEAGenome.h>

namespace SGA {

    MOPRHEAGenome::MOPRHEAGenome(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params)
    {
        const int playerID = gameState.getCurrentTBSPlayer();
        auto actionSpace = forwardModel.generateActions(gameState, playerID);

        size_t length = 0;
        while (!gameState.isGameOver && actionSpace.size() > 0 && length < params.INDIVIDUAL_LENGTH) {
            // choose a random portfolio and sample action
            const int portfolioIndex = rand() % params.portfolio.size();
            BaseActionScript* portfolio = params.portfolio.at(portfolioIndex).get();
            auto action = portfolio->getAction(gameState, actionSpace, playerID);
        	
            //todo forward random generator to getRandomAction
            applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
            actions.emplace_back(action);
            portfolioIndices.emplace_back(portfolioIndex);
            length++;
        }

        // rate newly created individual
		// todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.heuristic->evaluateGameState(forwardModel, gameState, playerID);    	
        value2 = params.heuristic2->evaluateGameState(forwardModel, gameState, playerID);    	
    }

    MOPRHEAGenome::MOPRHEAGenome(std::vector<Action>& actions, std::vector<int>& portfolioIndices, double value1, double value2) :
        actions(std::move(actions)), portfolioIndices(std::move(portfolioIndices)), value1(value1), value2(value2){}

    void MOPRHEAGenome::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action, MOPRHEAParameters& params)
    {
        params.REMAINING_FM_CALLS--;
        forwardModel.advanceGameState(gameState, action);
        while (gameState.getCurrentTBSPlayer() != params.PLAYER_ID && !gameState.isGameOver)
        {
            if (params.getOpponentModel()) // use default opponentModel to choose actions until the turn has ended
            {
                params.REMAINING_FM_CALLS--;
                auto opActionSpace = forwardModel.generateActions(gameState, gameState.getCurrentTBSPlayer());
                auto opAction = params.getOpponentModel()->getAction(gameState, opActionSpace, gameState.getCurrentTBSPlayer());
                forwardModel.advanceGameState(gameState, opAction);
            }
            else // skip opponent turn
            {
                forwardModel.advanceGameState(gameState, Action::createEndAction(gameState.getCurrentTBSPlayer()));
            }
        }
    	
        actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
    }
	
    void MOPRHEAGenome::mutate(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params, std::mt19937& randomGenerator)
    {
        const int playerID = gameState.getCurrentTBSPlayer();
        auto actionSpace = forwardModel.generateActions(gameState, playerID);

        // go through the actions and fill the actionVector of its child
        unsigned long long actIdx = 0;
        while (!gameState.isGameOver && actionSpace.size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
        	
            // replace with random portfolio in case of mutate or no portfolio available
            if (mutate || (actIdx < portfolioIndices.size()))
            {
                //todo use random generator to get a randomAction
                const int portfolioIndex = rand() % params.portfolio.size();
                BaseActionScript* portfolio = params.portfolio.at(portfolioIndex).get();
                auto action = portfolio->getAction(gameState, actionSpace, playerID);

                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
            	if (actIdx < actions.size())
            	{
                    actions[actIdx] = action;
                    portfolioIndices[actIdx] = portfolioIndex;
            	}
            	else
            	{
                    actions.emplace_back(action);
                    portfolioIndices.emplace_back(portfolioIndex);
            	}
            }
            else
            {
                // use previous portfolio but sample a new action
                if (actIdx < portfolioIndices.size())
                {
                    actions[actIdx] = params.portfolio[portfolioIndices[actIdx]]->getAction(gameState, actionSpace, playerID);
				}
            	else
				{
                    portfolioIndices.emplace_back(rand() % params.portfolio.size());
                    actions.emplace_back(params.portfolio[portfolioIndices[actIdx]]->getAction(gameState, actionSpace, playerID));
				}
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }
        	
        	actIdx++;
        }

        // rate mutated individual
        // todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.heuristic->evaluateGameState(forwardModel, gameState, playerID);
        value2 = params.heuristic2->evaluateGameState(forwardModel, gameState, playerID);
    }

    MOPRHEAGenome MOPRHEAGenome::crossover(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params, std::mt19937& randomGenerator, MOPRHEAGenome& parent1, MOPRHEAGenome& parent2)
    {
        // create a new individual and its own gameState copy
        const int playerID = gameState.getCurrentTBSPlayer();
        auto actionSpace = forwardModel.generateActions(gameState, playerID);

        // initialize variables for the new genome to be created
        std::vector<Action> actions;
        std::vector<int> portfolioIndices;
    	
        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;
        while (!gameState.isGameOver && actionSpace.size() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            // if mutate do a random mutation else apply uniform crossover
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
        	
            // mutation = randomly select a new action for gameStateCopy
            if (mutate)
            {
                const int portfolioIndex = rand() % params.portfolio.size();
                BaseActionScript* portfolio = params.portfolio.at(portfolioIndex).get();
                auto action = portfolio->getAction(gameState, actionSpace, playerID);

                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
                actions.emplace_back(action);
                portfolioIndices.emplace_back(portfolioIndex);
            }
            else
            {
                int chosenPortfolio;
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                MOPRHEAGenome& from = useParent1First ? parent1 : parent2;
            	// check the first parent and choose portfolio if available
                if (actIdx < from.portfolioIndices.size())
                {
                    chosenPortfolio = from.portfolioIndices[actIdx];
                }
            	else
                {
                    // check the second parent and choose portfolio if available
                    from = useParent1First ? parent2 : parent1;
                    if (actIdx < from.portfolioIndices.size())
                    {
                        chosenPortfolio = from.portfolioIndices[actIdx];
                    }
            		else
                    {
                        // use a random portfolio by default
                        chosenPortfolio = rand() % params.portfolio.size();
                    }
                }
                
                portfolioIndices.emplace_back(chosenPortfolio);
                actions.emplace_back(params.portfolio[chosenPortfolio]->getAction(gameState, actionSpace, playerID));
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }

            actIdx++;
        }
    	
        // todo both objectives should be minimized, remove the minus if this became more general
        double value1 = -params.heuristic->evaluateGameState(forwardModel, gameState, playerID);
        double value2 = params.heuristic2->evaluateGameState(forwardModel, gameState, playerID);
    	return MOPRHEAGenome(actions, portfolioIndices, value1, value2);
    }	

    void MOPRHEAGenome::shift(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params)
    {
        const int playerID = gameState.getCurrentTBSPlayer();

    	// reuse previous solution
        std::rotate(portfolioIndices.begin(), portfolioIndices.begin() + 1, portfolioIndices.end());
        std::rotate(actions.begin(), actions.begin() + 1, actions.end());
    	
        // replace last portfolio with a random new one
        portfolioIndices[portfolioIndices.size() - 1] = rand() % params.portfolio.size();
    	
		// check if actions are still applicable and if not sample a new one from portfolio
    	// always re-sample the last action since it is the rotated action from the previous solution
        auto actionSpace = forwardModel.generateActions(gameState, playerID);
    	for (size_t i = 0; i < actions.size(); i++)
    	{
            if (actionSpace.size() == 0)
                break;

            if (!actions[i].validate(gameState))
            {
                auto newAction = params.portfolio[portfolioIndices[i]]->getAction(gameState, actionSpace, playerID);
                actions[i] = newAction;
            }
    		
            applyActionToGameState(forwardModel, gameState, actionSpace, actions[i], params);
    	}

    	// re-evaluate the shifted individual
		// todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.heuristic->evaluateGameState(forwardModel, gameState, playerID);
        value2 = params.heuristic2->evaluateGameState(forwardModel, gameState, playerID);
    }

    void MOPRHEAGenome::toString() const
    {
        std::cout << "MOPortfolioRHEAGenome" << "\n";
        std::cout << "\tactions=" << "\n";
        for (Action action : actions)
        {
            std::cout << "\t\t" << action.ownerID << ";" << action.getSourceID() << ";" << action.targets[0].getEntityID() << "\n";
        }

        std::cout << "\tvalue1=" << value1 << "\n;";
        std::cout << "\tvalue2=" << value2 << "\n;";
    }

}
