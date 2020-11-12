#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAGenome.h>

#include "Agent/PortfolioRHEA/PortfolioRHEAGenome.h"

namespace SGA {

    MOPortfolioRHEAGenome::MOPortfolioRHEAGenome(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params)
    {
        const int playerID = gameState.currentPlayer;
        auto actionSpace = forwardModel.getActions(gameState);

        size_t length = 0;
        while (!gameState.isGameOver && actionSpace->size() > 0 && length < params.INDIVIDUAL_LENGTH) {
            // choose a random portfolio and sample action
            const int portfolioIndex = rand() % params.PORTFOLIO.size();
            BaseActionScript* portfolio = params.PORTFOLIO.at(portfolioIndex).get();
            const Action action = portfolio->getAction(gameState, actionSpace);
        	
            //todo forward random generator to getRandomAction
            applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
            actions.emplace_back(action);
            portfolioIndices.emplace_back(portfolioIndex);
            length++;
        }

        // rate newly created individual
		// todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.HEURISTIC1.evaluateGameState(forwardModel, gameState, playerID);    	
        value2 = params.HEURISTIC2.evaluateGameState(forwardModel, gameState, playerID);    	
    }

    MOPortfolioRHEAGenome::MOPortfolioRHEAGenome(std::vector<Action<Vector2i>>& actions, std::vector<int>& portfolioIndices, double value1, double value2) :
        actions(std::move(actions)), portfolioIndices(std::move(portfolioIndices)), value1(value1), value2(value2){}

    void MOPortfolioRHEAGenome::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, const Action<Vector2i>& action, MOPortfolioRHEAParams& params)
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
	
    void MOPortfolioRHEAGenome::mutate(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params, std::mt19937& randomGenerator)
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
            if (mutate || (actIdx < portfolioIndices.size()))
            {
                //todo use random generator to get a randomAction
                const int portfolioIndex = rand() % params.PORTFOLIO.size();
                BaseActionScript* portfolio = params.PORTFOLIO.at(portfolioIndex).get();
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
                    actions[actIdx] = params.PORTFOLIO[portfolioIndices[actIdx]]->getAction(gameState, actionSpace);
				}
            	else
				{
                    portfolioIndices.emplace_back(rand() % params.PORTFOLIO.size());
                    actions.emplace_back(params.PORTFOLIO[portfolioIndices[actIdx]]->getAction(gameState, actionSpace));
				}
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }
        	
        	actIdx++;
        }

        // rate mutated individual
        // todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.HEURISTIC1.evaluateGameState(forwardModel, gameState, playerID);
        value2 = params.HEURISTIC2.evaluateGameState(forwardModel, gameState, playerID);
    }

    MOPortfolioRHEAGenome MOPortfolioRHEAGenome::crossover(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params, std::mt19937& randomGenerator, MOPortfolioRHEAGenome& parent1, MOPortfolioRHEAGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.getActions(gameState);
        const int playerID = gameState.currentPlayer;

        // initialize variables for the new genome to be created
        std::vector<Action<Vector2i>> actions;
        std::vector<int> portfolioIndices;
    	
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
                const int portfolioIndex = rand() % params.PORTFOLIO.size();
                BaseActionScript* portfolio = params.PORTFOLIO.at(portfolioIndex).get();
                Action action = portfolio->getAction(gameState, actionSpace);

                applyActionToGameState(forwardModel, gameState, actionSpace, action, params);
                actions.emplace_back(action);
                portfolioIndices.emplace_back(portfolioIndex);
            }
            else
            {
                int chosenPortfolio;
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                MOPortfolioRHEAGenome& from = useParent1First ? parent1 : parent2;
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
                        chosenPortfolio = rand() % params.PORTFOLIO.size();
                    }
                }
                
                portfolioIndices.emplace_back(chosenPortfolio);
                actions.emplace_back(params.PORTFOLIO[chosenPortfolio]->getAction(gameState, actionSpace));
                applyActionToGameState(forwardModel, gameState, actionSpace, actions[actIdx], params);
            }

            actIdx++;
        }
    	
        // todo both objectives should be minimized, remove the minus if this became more general
        double value1 = -params.HEURISTIC1.evaluateGameState(forwardModel, gameState, playerID);
        double value2 = params.HEURISTIC2.evaluateGameState(forwardModel, gameState, playerID);
    	return MOPortfolioRHEAGenome(actions, portfolioIndices, value1, value2);
    }	

    void MOPortfolioRHEAGenome::shift(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params)
    {
        const int playerID = gameState.currentPlayer;

    	// reuse previous solution
        std::rotate(portfolioIndices.begin(), portfolioIndices.begin() + 1, portfolioIndices.end());
        std::rotate(actions.begin(), actions.begin() + 1, actions.end());
    	
        // replace last portfolio with a random new one
        portfolioIndices[portfolioIndices.size() - 1] = rand() % params.PORTFOLIO.size();
    	
		// check if actions are still applicable and if not sample a new one from portfolio
    	// always re-sample the last action since it is the rotated action from the previous solution
        auto actionSpace = forwardModel.getActions(gameState);
    	for (size_t i = 0; i < actions.size(); i++)
    	{
            if (actionSpace->size() == 0)
                break;

            if (!forwardModel.isValid(gameState, actions[i]))
            {
                const Action newAction = params.PORTFOLIO[portfolioIndices[i]]->getAction(gameState, actionSpace);
                actions[i] = newAction;
            }
    		
            applyActionToGameState(forwardModel, gameState, actionSpace, actions[i], params);
    	}

    	// re-evaluate the shifted individual
		// todo both objectives should be minimized, remove the minus if this became more general
        value1 = -params.HEURISTIC1.evaluateGameState(forwardModel, gameState, playerID);
        value2 = params.HEURISTIC2.evaluateGameState(forwardModel, gameState, playerID);
    }

    void MOPortfolioRHEAGenome::toString() const
    {
        std::cout << "MOPortfolioRHEAGenome" << "\n";
        std::cout << "\tactions=" << "\n";
        for (Action<Vector2i> action : actions)
        {
            std::cout << "\t\t" << action.getPlayerID() << ";" << action.getSourceUnitID() << ";" << action.getTargetUnitID() << "\n";
        }

        std::cout << "\tvalue1=" << value1 << "\n;";
        std::cout << "\tvalue2=" << value2 << "\n;";
    }

}
