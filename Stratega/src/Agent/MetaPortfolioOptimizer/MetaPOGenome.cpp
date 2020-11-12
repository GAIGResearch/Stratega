#include <Agent/MetaPortfolioOptimizer/MetaPOGenome.h>

namespace SGA {


	
    MetaPOGenome::MetaPOGenome(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params)
    {
        auto actionSpace = forwardModel.getActions(gameState);

        size_t length = 0;
        std::vector<TBSUnit*> units = gameState.getPlayer(gameState.currentPlayer)->getUnits();
        unitScript = std::map<int, BaseActionScript*>();
        scriptUpdates = std::vector<UnitScriptChange>();
    	
    	// assign a random script to each unit
        std::set<int> playerUnits = std::set<int>();
    	for (TBSUnit* unit : units)
    	{
    		if (unit->getPlayerID() == gameState.currentPlayer)
    		{
				unitScript[unit->getUnitID()] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
                playerUnits.emplace(unit->getUnitID());
            }
    	}

    	// create random UnitScriptChanges
    	for (int i = 0; i < params.NR_OF_SCRIPT_CHANGES; i++)
    	{
            scriptUpdates.emplace_back(UnitScriptChange::createRandomScriptChange(params.HORIZON, playerUnits, params.PORTFOLIO.size()));
    	}
    	
        // rate newly created individual
        value = evaluateGenome(forwardModel, gameState, params);
    }

    double MetaPOGenome::evaluateGenome(TBSForwardModel& forwardModel, TBSGameState& gameState, MetaPOParams& params)
    {
        const int startTurn = gameState.currentGameTurn;
        int lastKnownTurn = gameState.currentGameTurn;
        const int playerID = gameState.currentPlayer;
    	
        std::map<int, BaseActionScript*> tmpMap(unitScript);
    	
    	while (!gameState.isGameOver && gameState.currentGameTurn - startTurn < params.HORIZON)
    	{
            auto actionSpace = forwardModel.getActions(gameState);
            params.REMAINING_FM_CALLS--;
    		if (gameState.currentPlayer == playerID)
    		{
				// choose action according to unitScript
                int nextUnit = actionSpace.at(0).getSourceUnitID();
                if (tmpMap.contains(nextUnit))
                {
                    auto action = tmpMap[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
                    forwardModel.advanceGameState(gameState, action);
                } else
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
                    std::vector<SGA::Action<Vector2i>> endTurnActionSpace;
                    forwardModel.generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
                    forwardModel.advanceGameState(gameState, endTurnActionSpace.at(0));
            	}
            }

    		if (gameState.currentGameTurn != lastKnownTurn)
    		{
                const int turnDifference = gameState.currentGameTurn - startTurn;
    			for (UnitScriptChange scriptChange : scriptUpdates)
    			{
    				if (scriptChange.tick == turnDifference)
    				{
                        tmpMap[scriptChange.unitID] = params.PORTFOLIO[scriptChange.targetScript].get();
    				}
    			}
                lastKnownTurn = gameState.currentGameTurn;
    		}
    	}
        return params.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
    }
	
    MetaPOGenome::MetaPOGenome(std::map<int, BaseActionScript*>& unitScript, std::vector<UnitScriptChange> scriptChanges, TBSForwardModel& forwardModel, TBSGameState& gameState, MetaPOParams& params) :
        unitScript(std::move(unitScript)), scriptUpdates(std::move(scriptChanges))
    {
        value = evaluateGenome(forwardModel, gameState, params);
    }

    Action<Vector2i> MetaPOGenome::getAction(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace)
    {
        const int nextUnit = actionSpace.at(0).getSourceUnitID();
        if (unitScript.contains(nextUnit))
        {
            return unitScript[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
        }
        return actionSpace.at(rand() % actionSpace.size());
    };
	
    void MetaPOGenome::mutate(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::mt19937& randomGenerator)
    {
        std::vector<TBSUnit*> units = gameState.getPlayer(gameState.currentPlayer)->getUnits();

        // go through the units and mutate some of their script assignments
        const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

        std::set<int> unitIDs = std::set<int>();
    	for (TBSUnit* unit : units)
    	{
            const int unitID = unit->getUnitID();
            unitIDs.emplace(unitID);
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
    		if (mutate || !unitScript.contains(unitID))
    		{
                unitScript[unitID] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
    		}
    	}

    	for (UnitScriptChange unitScriptChange : scriptUpdates)
    	{
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
            if (mutate)
            {
                unitScriptChange.mutate(params.HORIZON, unitIDs, params.PORTFOLIO.size(), randomGenerator);
            }
    	}
    	
        // rate mutated individual
        value = evaluateGenome(forwardModel, gameState, params);
    }

    MetaPOGenome MetaPOGenome::crossover(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::mt19937& randomGenerator, MetaPOGenome& parent1, MetaPOGenome& parent2)
    {
        // initialize variables for the new genome to be created
        std::map<int, BaseActionScript*> unitScript = std::map<int, BaseActionScript*>();

        std::vector<TBSUnit*> units = gameState.getPlayer(gameState.currentPlayer)->getUnits();

        // go through the units and mutate some of their script assignments
        const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

        for (TBSUnit* unit : units)
        {
            const bool mutate = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
            const int currentUnitID = unit->getUnitID();
        	
            if (mutate)
            {
                unitScript[currentUnitID] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
            }
			else
            {
                const bool useParent1First = doubleDistribution_(randomGenerator) < 0.5;
                MetaPOGenome& from = useParent1First ? parent1 : parent2;
                // check the first parent and choose portfolio if available
                if (from.unitScript.contains(currentUnitID))
                {
                    unitScript[currentUnitID] = from.unitScript[currentUnitID];
                }
                else
                {
                    // check the second parent and choose portfolio if available
                    from = useParent1First ? parent2 : parent1;
                    if (from.unitScript.contains(currentUnitID))
                    {
                        unitScript[currentUnitID] = from.unitScript[currentUnitID];
                    }
                    else
                    {
                        // use a random portfolio by default
                        unitScript[currentUnitID] = params.PORTFOLIO.at(rand() % params.PORTFOLIO.size()).get();
                    }
                }
            }
        }

        // create a new list of script updates by choosing script updates from our parents
        std::vector<UnitScriptChange> scriptChanges = std::vector<UnitScriptChange>();
    	for (int i = 0; i < parent1.scriptUpdates.size(); i++)
    	{
            const bool fromParent1 = doubleDistribution_(randomGenerator) < params.MUTATION_RATE;
            MetaPOGenome& from = fromParent1 ? parent1 : parent2;
            scriptChanges.emplace_back(from.scriptUpdates[i]);
    	}

    	return MetaPOGenome(unitScript, scriptChanges, forwardModel, gameState, params);
    }	

    void MetaPOGenome::shift(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::set<int>& unitIDs, std::mt19937& randomGenerator)
    {
    	for (UnitScriptChange& unitScriptChange : scriptUpdates)
    	{
            unitScriptChange.tick--;
    		if (unitScriptChange.tick == 0)
    		{
                unitScript[unitScriptChange.unitID] = params.PORTFOLIO[unitScriptChange.targetScript].get();
                unitScriptChange.mutateAll(params.HORIZON, unitIDs, params.PORTFOLIO.size(), randomGenerator);
    		}
    	}
    	
        value = evaluateGenome(forwardModel, gameState, params);
    }

    void MetaPOGenome::toString() const
    {
        std::cout << "PortfolioRHEAGenome" << "\n";
        std::cout << "\tUnit Script Assignments =" << "\n";
        for (std::pair<const int, BaseActionScript*> unitAssignment : unitScript)
        {
            std::cout << "\t\tUnit " <<  unitAssignment.first << ";" << unitAssignment.second << "\n";
        }

        std::cout << "\tvalue=" << value << "\n;";
    }

}
