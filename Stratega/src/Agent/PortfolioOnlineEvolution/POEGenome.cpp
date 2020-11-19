#include <Agent/PortfolioOnlineEvolution/POEGenome.h>

namespace SGA {

    POEGenome::POEGenome(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params)
    {
        const int playerID = gameState.currentPlayer;
        auto actionSpace = forwardModel.generateActions(gameState);

        size_t length = 0;
        std::vector<TBSUnit*> units = gameState.getPlayer(gameState.currentPlayer)->getUnits();
        std::set<int> playerUnits = std::set<int>();
        for (TBSUnit* unit : units)
        {
            playerUnits.emplace(unit->getUnitID());
        }
    	
        while (length < params.INDIVIDUAL_LENGTH) 
        {
            // choose a random portfolio for each unit
            std::map<int, BaseActionScript*> turnScriptAssignment;
            for (int unitID : playerUnits)
            {
                //todo forward random generator to getRandomPortfolio
                turnScriptAssignment[unitID] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
            }
        	
            scriptAssignment.emplace_back(turnScriptAssignment);
            length++;
        }
    	
        // rate newly created individual
        value = evaluateGenome(forwardModel, gameState, params);
    }

    double POEGenome::evaluateGenome(TBSForwardModel& forwardModel, TBSGameState& gameState, POEParams& params)
    {
        const int startTurn = gameState.currentGameTurn;
        const int playerID = gameState.currentPlayer;

        while (!gameState.isGameOver && gameState.currentGameTurn-startTurn < params.INDIVIDUAL_LENGTH)
        {
            auto actionSpace = forwardModel.generateActions(gameState);
            params.REMAINING_FM_CALLS--;
            std::map<int, BaseActionScript*> playerMap = scriptAssignment[gameState.currentGameTurn - startTurn];
            if (gameState.currentPlayer == playerID)
            {
                // choose action according to unitScript
                int nextUnit = actionSpace.at(0).sourceUnitID;
                if (playerMap.contains(nextUnit))
                {
                    auto action = playerMap[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
                    forwardModel.advanceGameState(gameState, action);
                }
                else
                {
                    forwardModel.advanceGameState(gameState, actionSpace.at(0));
                }
            }
            else
            {
                // apply OpponentModel
                if (params.opponentModel)
                {
                    auto action = params.opponentModel->getAction(gameState, actionSpace);
                    forwardModel.advanceGameState(gameState, action);
                }
                else
                {
                    forwardModel.advanceGameState(gameState, forwardModel.getActionSpace().generateEndOfTurnAction(gameState, gameState.currentPlayer));
                }
            }
        }
        return params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
    }

    POEGenome::POEGenome(std::vector<std::map<int, BaseActionScript*>> scriptAssignment, TBSForwardModel& forwardModel, TBSGameState& gameState, POEParams& params) :
        scriptAssignment(std::move(scriptAssignment))
    {
        value = evaluateGenome(forwardModel, gameState, params);
    }

    void POEGenome::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, const TBSAction& action, POEParams& params)
    {
        params.REMAINING_FM_CALLS--;
        forwardModel.advanceGameState(gameState, action);
        while (gameState.currentPlayer != params.PLAYER_ID && !gameState.isGameOver)
        {
            if (params.opponentModel) // use default opponentModel to choose actions until the turn has ended
            {
                params.REMAINING_FM_CALLS--;
                auto opActionSpace = forwardModel.generateActions(gameState);
                auto opAction = params.opponentModel->getAction(gameState, opActionSpace);
                forwardModel.advanceGameState(gameState, opAction);
            }
            else // skip opponent turn
            {
                forwardModel.advanceGameState(gameState, forwardModel.getActionSpace().generateEndOfTurnAction(gameState, gameState.currentPlayer));
            }
        }

        actionSpace = forwardModel.generateActions(gameState);
    }

    void POEGenome::mutate(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params, std::mt19937& randomGenerator)
    {
        auto actionSpace = forwardModel.generateActions(gameState);
        const int playerID = gameState.currentPlayer;

        const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
        for (std::map<int, BaseActionScript*>& assignmentPerTurn : scriptAssignment)
        {
            for (auto& entry : assignmentPerTurn)
            {
                const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
                if (mutate)
                {
                    entry.second = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
                }
			}
        }

        // rate mutated individual
        this->value = evaluateGenome(forwardModel, gameState, params);
    }

    TBSAction POEGenome::getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace)
    {
        const int nextUnit = actionSpace.at(0).sourceUnitID;
        if (scriptAssignment[0].contains(nextUnit))
        {
            return scriptAssignment[0][nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
        }
        return actionSpace.at(rand() % actionSpace.size());
    };
	
    POEGenome POEGenome::crossover(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params, std::mt19937& randomGenerator, POEGenome& parent1, POEGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.generateActions(gameState);
        const int playerID = gameState.currentPlayer;

        // initialize variables for the new genome to be created
        std::vector<std::map<int, BaseActionScript*>> tmpScriptAssignment;

        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;

    	// determine unit IDs
        std::vector<TBSUnit*> units = gameState.getPlayer(gameState.currentPlayer)->getUnits();
        std::set<int> playerUnits = std::set<int>();
        for (TBSUnit* unit : units)
        {
            playerUnits.emplace(unit->getUnitID());
        }

        std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
    	for (size_t i = 0; i < params.INDIVIDUAL_LENGTH; i++)
    	{
            std::map<int, BaseActionScript*> turnScriptAssignment;
            for (int unitID : playerUnits)
            {
                const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
                if (mutate)
                {
                    // choose a random portfolio
                    //todo forward random generator to getRandomPortfolio
                    turnScriptAssignment[unitID] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
                }
                else 
                {
                    BaseActionScript* chosenPortfolio = nullptr;
                	
                    // select a portfolio from its parents
                    const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                    POEGenome& from = useParent1First ? parent1 : parent2;
                	
                    // check the first parent and choose portfolio if available
                    if (i < from.scriptAssignment.size() && from.scriptAssignment[i].contains(unitID))
                    {
                        chosenPortfolio = from.scriptAssignment[i][unitID];
                    }
                    else
                    {
                        // check the second parent and choose portfolio if available
                        from = useParent1First ? parent2 : parent1;
                        if (actIdx < from.scriptAssignment[i].contains(unitID))
                        {
                            chosenPortfolio = from.scriptAssignment[i][unitID];
                        }
                        else
                        {
                            // use a random portfolio by default
                            chosenPortfolio = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
                        }
                    }
                    turnScriptAssignment[unitID] = chosenPortfolio;
                }
            }

    		// add turnScriptAssignment to the list of script assignments
            tmpScriptAssignment.emplace_back(turnScriptAssignment);  
    	}

    	// create a new genome which will be automatically evaluated in its constructor
        return POEGenome(tmpScriptAssignment, forwardModel, gameState, params);
    }

    void POEGenome::shift(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params)
    {
        const int playerID = gameState.currentPlayer;

        // reuse previous solution
        std::rotate(scriptAssignment.begin(), scriptAssignment.begin() + 1, scriptAssignment.end());

        // replace last scriptAssignment with a random new one
        auto& lastScriptAssignment = scriptAssignment[scriptAssignment.size() - 1];
        for (auto& entry : lastScriptAssignment)
        {
            //todo forward random generator to getRandomPortfolio
            lastScriptAssignment[entry.first] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
        }
    	
        // re-evaluate the shifted individual
        value = evaluateGenome(forwardModel, gameState, params);
    }

    void POEGenome::toString() const
    {
        std::cout << "POEGenome" << "\n";
        std::cout << "\tscriptAssignment=" << "\n";
        int i = 0;
        for (const std::map<int, BaseActionScript*>& assignmentPerTurn : scriptAssignment)
        {
            std::cout << "\tTurn: " << i++ << std::endl;
            for (const auto& entry : assignmentPerTurn)
            {
                std::cout << "\t\tUnitID: " << entry.first << "; Script:" << entry.second->toString() << std::endl;
            }
        }

        std::cout << "\tvalue=" << value << "\n;";
    }

}
