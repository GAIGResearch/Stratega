#include <Stratega/Agent/POEAgent/POEGenome.h>


namespace SGA {

    POEGenome::POEGenome(const ForwardModel& forwardModel, GameState gameState, POEParams& params)
    {
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);

        size_t length = 0;
        std::vector<Entity*> units = gameState.getPlayerEntities(params.PLAYER_ID);
        std::set<int> playerUnits = std::set<int>();
        for (Entity* unit : units)
        {
            playerUnits.emplace(unit->id);
        }
    	
        while (length < params.INDIVIDUAL_LENGTH) 
        {
            // choose a random portfolio for each unit
            std::map<int, BaseActionScript*> turnScriptAssignment;
            for (int unitID : playerUnits)
            {
                //todo forward random generator to getRandomPortfolio
                turnScriptAssignment[unitID] = params.portfolio.at(rand() % params.portfolio.size()).get();
            }
        	
            scriptAssignment.emplace_back(turnScriptAssignment);
            length++;
        }
    	
        // rate newly created individual
        value = evaluateGenome(forwardModel, gameState, params);
    }

    double POEGenome::evaluateGenome(const ForwardModel& forwardModel, GameState& gameState, POEParams& params)
    {
        const int startTurn = gameState.currentTick;
        const int playerID = params.PLAYER_ID;

        while (!gameState.isGameOver && gameState.currentTick-startTurn < params.INDIVIDUAL_LENGTH)
        {
            auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);
            params.REMAINING_FM_CALLS--;
            std::map<int, BaseActionScript*> playerMap = scriptAssignment[gameState.currentTick - startTurn];
            if (gameState.getCurrentTBSPlayer() == playerID)
            {
                // choose action according to unitScript
                int nextUnit = actionSpace.at(0).getSourceID();
                if (playerMap.contains(nextUnit) && actionSpace.at(0).actionTypeFlags != ActionFlag::EndTickAction)
                {
                    auto action = playerMap[nextUnit]->getActionForUnit(gameState, actionSpace, gameState.getEntityConst(nextUnit)->ownerID, nextUnit);
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
                if (params.getOpponentModel())
                {
                    auto action = params.getOpponentModel()->getAction(gameState, actionSpace, gameState.getCurrentTBSPlayer());
                    forwardModel.advanceGameState(gameState, action);
                }
                else
                {
                    forwardModel.advanceGameState(gameState, Action::createEndAction(gameState.getCurrentTBSPlayer()));
                }
            }
        }
        return params.heuristic->evaluateGameState(forwardModel, gameState, playerID);
    }

    POEGenome::POEGenome(std::vector<std::map<int, BaseActionScript*>> scriptAssignment, const ForwardModel& forwardModel, GameState& gameState, POEParams& params) :
        scriptAssignment(std::move(scriptAssignment))
    {
        value = evaluateGenome(forwardModel, gameState, params);
    }

    void POEGenome::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action, POEParams& params)
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

    void POEGenome::mutate(const ForwardModel& forwardModel, GameState gameState, POEParams& params, std::mt19937& randomGenerator)
    {
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);

        const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
        for (std::map<int, BaseActionScript*>& assignmentPerTurn : scriptAssignment)
        {
            for (auto& entry : assignmentPerTurn)
            {
                const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
                if (mutate)
                {
                    entry.second = params.portfolio.at(rand() % params.portfolio.size()).get();
                }
			}
        }

        // rate mutated individual
        this->value = evaluateGenome(forwardModel, gameState, params);
    }

    Action POEGenome::getAction(GameState& gameState, std::vector<Action>& actionSpace)
    {
        const int nextUnit = actionSpace.at(0).getSourceID();
        if (scriptAssignment[0].contains(nextUnit))
        {
            
            return scriptAssignment[0][nextUnit]->getActionForUnit(gameState, actionSpace, gameState.getEntityConst(nextUnit)->ownerID, nextUnit);
        }
        return actionSpace.at(rand() % actionSpace.size());
    };
	
    POEGenome POEGenome::crossover(const ForwardModel& forwardModel, GameState gameState, POEParams& params, std::mt19937& randomGenerator, POEGenome& parent1, POEGenome& parent2)
    {
        // create a new individual and its own gameState copy
        auto actionSpace = forwardModel.generateActions(gameState, params.PLAYER_ID);

        // initialize variables for the new genome to be created
        std::vector<std::map<int, BaseActionScript*>> tmpScriptAssignment;

        // step-wise add actions by mutation or crossover
        size_t actIdx = 0;

    	// determine unit IDs
        std::vector<Entity*> units = gameState.getPlayerEntities(params.PLAYER_ID);
        std::set<int> playerUnits = std::set<int>();
        for (Entity* unit : units)
        {
            playerUnits.emplace(unit->id);
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
                    turnScriptAssignment[unitID] = params.portfolio.at(rand() % params.portfolio.size()).get();
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
                            chosenPortfolio = params.portfolio.at(rand() % params.portfolio.size()).get();
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

    void POEGenome::shift(const ForwardModel& forwardModel, GameState gameState, POEParams& params)
    {
        const int playerID = params.PLAYER_ID;

        // reuse previous solution
        std::rotate(scriptAssignment.begin(), scriptAssignment.begin() + 1, scriptAssignment.end());

        // replace last scriptAssignment with a random new one
        auto& lastScriptAssignment = scriptAssignment[scriptAssignment.size() - 1];
        for (auto& entry : lastScriptAssignment)
        {
            //todo forward random generator to getRandomPortfolio
            lastScriptAssignment[entry.first] = params.portfolio.at(rand() % params.portfolio.size()).get();
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
