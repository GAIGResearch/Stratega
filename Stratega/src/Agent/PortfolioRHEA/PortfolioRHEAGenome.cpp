#include <Agent/PortfolioRHEA/PortfolioRHEAGenome.h>

namespace SGA {

    PortfolioRHEAGenome::PortfolioRHEAGenome(TBSForwardModel& forwardModel, TBSGameState gameState, PortfolioRHEAParams& params)
    {
        auto actionSpace = forwardModel.getActions(gameState);

        size_t length = 0;
        while (!gameState.isGameOver && actionSpace->count() > 0 && length < params.INDIVIDUAL_LENGTH) {
            // choose a random portfolio and sample action
            const int portfolioIndex = rand() % params.portfolios.size();
            BasePortfolio* portfolio = params.portfolios.at(portfolioIndex).get();
            const Action action = portfolio->getAction(gameState, actionSpace);
        	
            //todo forward random generator to getRandomAction
            applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
            actions.emplace_back(action);
            portfolioIndices.emplace_back(portfolioIndex);
            length++;
        }

        // rate newly created individual
        value = params.HEURISTIC.evaluateGameState(forwardModel, gameState);    	
    }

    PortfolioRHEAGenome::PortfolioRHEAGenome(std::vector<Action<Vector2i>>& actions, std::vector<int>& portfolioIndices, double value) :
        actions(std::move(actions)), portfolioIndices(std::move(portfolioIndices)), value(value){}

    void PortfolioRHEAGenome::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, const Action<Vector2i>& action, PortfolioRHEAParams& params)
    {
        params.REMAINING_FM_CALLS--;
        forwardModel.advanceGameState(gameState, action);
        while (gameState.currentPlayer != params.PLAYER_ID)
        {
            ActionSpace<Vector2i> endTurnActionSpace;
            forwardModel.generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
            forwardModel.advanceGameState(gameState, endTurnActionSpace.getAction(0));
        }
    	
        actionSpace = forwardModel.getActions(gameState);
    }
	
    void PortfolioRHEAGenome::mutate(TBSForwardModel& forwardModel, TBSGameState gameState, PortfolioRHEAParams& params, std::mt19937& randomGenerator)
    {
        auto actionSpace = forwardModel.getActions(gameState);

        // go through the actions and fill the actionVector of its child
        unsigned long long actIdx = 0;
        while (!gameState.isGameOver && actionSpace->count() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
        	
            // replace with random portfolio in case of mutate or no portfolio available
            if (mutate || (actIdx < portfolioIndices.size()))
            {
                //todo use random generator to get a randomAction
                const int portfolioIndex = rand() % params.portfolios.size();
                BasePortfolio* portfolio = params.portfolios.at(portfolioIndex).get();
                const Action action = portfolio->getAction(gameState, actionSpace);

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
                    actions[actIdx] = params.portfolios[portfolioIndices[actIdx]]->getAction(gameState, actionSpace);
				}
            	else
				{
                    portfolioIndices.emplace_back(rand() % params.portfolios.size());
                    actions.emplace_back(params.portfolios[portfolioIndices[actIdx]]->getAction(gameState, actionSpace));
				}
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }
        	
        	actIdx++;
        }

        // rate mutated individual
        this->value = params.HEURISTIC.evaluateGameState(forwardModel, gameState);
    }

    PortfolioRHEAGenome PortfolioRHEAGenome::crossover(TBSForwardModel& forwardModel, TBSGameState gameState, PortfolioRHEAParams& params, std::mt19937& randomGenerator, PortfolioRHEAGenome& parent1, PortfolioRHEAGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.getActions(gameState);

        // initialize variables for the new genome to be created
        std::vector<Action<Vector2i>> actions;
        std::vector<int> portfolioIndices;
    	
        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;
        while (!gameState.isGameOver && actionSpace->count() > 0 && actIdx < params.INDIVIDUAL_LENGTH)
        {
            // if mutate do a random mutation else apply uniform crossover
            std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
        	
            // mutation = randomly select a new action for gameStateCopy
            if (mutate)
            {
                const int portfolioIndex = rand() % params.portfolios.size();
                BasePortfolio* portfolio = params.portfolios.at(portfolioIndex).get();
                Action action = portfolio->getAction(gameState, actionSpace);

                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
                actions.emplace_back(action);
                portfolioIndices.emplace_back(portfolioIndex);
            }
            else
            {
                int chosenPortfolio;
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                PortfolioRHEAGenome& from = useParent1First ? parent1 : parent2;
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
                        chosenPortfolio = rand() % params.portfolios.size();
                    }
                }
                
                portfolioIndices.emplace_back(chosenPortfolio);
                actions.emplace_back(params.portfolios[chosenPortfolio]->getAction(gameState, actionSpace));
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }

            actIdx++;
        }
    	
        const double value = params.HEURISTIC.evaluateGameState(forwardModel, gameState);
        return PortfolioRHEAGenome(actions, portfolioIndices, value);
    }	

    void PortfolioRHEAGenome::shift(TBSForwardModel& forwardModel, TBSGameState gameState, PortfolioRHEAParams& params)
    {
    	// reuse previous solution
        std::rotate(portfolioIndices.begin(), portfolioIndices.begin() + 1, portfolioIndices.end());
        std::rotate(actions.begin(), actions.begin() + 1, actions.end());
    	
        // replace last portfolio with a random new one
        portfolioIndices[portfolioIndices.size() - 1] = rand() % params.portfolios.size();
    	
		// check if actions are still applicable and if not sample a new one from portfolio
    	// always re-sample the last action since it is the rotated action from the previous solution
        auto actionSpace = forwardModel.getActions(gameState);
    	for (size_t i = 0; i < actions.size(); i++)
    	{
            if (actionSpace->count() == 0)
                break;

            if (!forwardModel.isValid(gameState, actions[i]))
            {
                const Action newAction = params.portfolios[portfolioIndices[i]]->getAction(gameState, actionSpace);
                actions[i] = newAction;
            }
    		
            applyActionToGameState(forwardModel, gameState, actionSpace, actions[i], params);
    	}

    	// re-evaluate the shifted individual
        value = params.HEURISTIC.evaluateGameState(forwardModel, gameState);
    }

    void PortfolioRHEAGenome::toString() const
    {
        std::cout << "PortfolioRHEAGenome" << "\n";
        std::cout << "\tactions=" << "\n";
        for (Action action : actions)
        {
            std::cout << "\t\t" << action.getPlayerID() << ";" << action.getSourceUnitID() << ";" << action.getTargetUnitID() << "\n";
        }

        std::cout << "\tvalue=" << value << "\n;";
    }

}
