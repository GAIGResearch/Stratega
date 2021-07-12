#include <Stratega/Agent/PortfolioGreedySearchAgent.h>


namespace SGA
{
	//todo only works for 2 players
	ActionAssignment PortfolioGreedySearchAgent::computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs)
	{
		if (state.gameType != GameType::TBS)
		{
			throw std::runtime_error("PGSAgent only supports TBS-Games");
		}
		
		auto actionSpace = forwardModel->generateActions(state, getPlayerID());

		if (actionSpace.size() == 1)
		{
			return ActionAssignment::fromSingleAction(actionSpace.at(0));
		}
		else
		{
			params_.REMAINING_FM_CALLS = params_.MAX_FM_CALLS;
					
			// retrieving unit sets
			std::vector<Entity*> playerUnits = state.getPlayerEntities(getPlayerID());
			std::vector<Entity*> opponentUnits = state.getNonPlayerEntities(getPlayerID());

			// initializing script assignments
			std::map<int, BaseActionScript*> playerPortfolios = std::map<int, BaseActionScript*>();
			std::map<int, BaseActionScript*> opponentPortfolios = std::map<int, BaseActionScript*>();

			// sets a random portfolio for each player
			InitializePortfolios(playerUnits, playerPortfolios);
			InitializePortfolios(opponentUnits, opponentPortfolios);

			// improve the portfolio until the number of available forward model calls has been used
			while (params_.REMAINING_FM_CALLS > 0)
			{
				Improve(*forwardModel, state, opponentUnits, opponentPortfolios, playerPortfolios);
				if (params_.REMAINING_FM_CALLS <= 0)
					break;

				Improve(*forwardModel, state, playerUnits, playerPortfolios, opponentPortfolios);
			}

			std::cout << "action returned by PGS" << std::endl;
			// return the first action of the player's portfolio
			return ActionAssignment::fromSingleAction(GetPortfolioAction(state, actionSpace, playerPortfolios, opponentPortfolios));
		}
	}

	void PortfolioGreedySearchAgent::init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs)
	{
		params_.PLAYER_ID = getPlayerID();
		if (params_.heuristic == nullptr)
		{
			params_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
		}

	}
	
	// initialize the player's portfolio map with random scripts
	void PortfolioGreedySearchAgent::InitializePortfolios(std::vector<Entity*>& units, std::map<int, BaseActionScript*>& portfolioMap)
	{
		for (Entity* u : units)
		{
			portfolioMap[u->id] = params_.portfolio[rand() % params_.portfolio.size()].get();
		}
	}

	void PortfolioGreedySearchAgent::Improve(const ForwardModel& forwardModel, GameState& gameState, std::vector<Entity*>& playerUnits, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios)
	{		
		double currentPortfolioValue = Playout(forwardModel, gameState, playerPortfolios, opponentPortfolios);

		for (int i = 0; i < params_.ITERATIONS_PER_IMPROVE; i++)
		{
			for (auto unit : playerUnits)
			{
				int unitId = unit->id;
				BaseActionScript* previousBestScript = playerPortfolios[unitId];
				double bestNewScriptValue = -std::numeric_limits<double>::max();
				int bestScriptIndex = -1;

				// search for best script for the current unit
				for (int i = 0; i < params_.portfolio.size(); i++)
				{
					//if (params_.PORTFOLIO[i].get() == previousBestScript) 
					//	continue;	// skip the playout from the previousBestScript
					
					playerPortfolios[unitId] = params_.portfolio[i].get();
					const double currentScriptValue = Playout(forwardModel, gameState, playerPortfolios, opponentPortfolios);
					if (currentScriptValue > bestNewScriptValue)
					{
						bestScriptIndex = i;
						bestNewScriptValue = currentScriptValue;
					} 
				}

				// update portfolio and its value
				if (bestNewScriptValue > currentPortfolioValue)
				{
					// in case we found a better script, permanently replace the old one
					playerPortfolios[unitId] = params_.portfolio[bestScriptIndex].get();
					currentPortfolioValue = bestNewScriptValue;
				}
				else
				{
					// if no script has been better than the previous best one, we just reset the portfolio
					playerPortfolios[unitId] = previousBestScript;
				}

				if (params_.REMAINING_FM_CALLS <= 0)
					return;
			}
		}
	}

	// Simulated the players PORTFOLIO until a maximum number of turns has been simulated or the game has ended
	// returns the heuristic value of the simulated game state
	double PortfolioGreedySearchAgent::Playout(const ForwardModel& forwardModel, GameState gameState, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios)
	{
		const int playerID = gameState.getCurrentTBSPlayer();
		auto actionSpace = forwardModel.generateActions(gameState, gameState.getCurrentTBSPlayer());
		int simulatedTurns = 0;
		while (simulatedTurns < params_.NR_OF_TURNS_PLANNED && !gameState.isGameOver && actionSpace.size()!=0)
		{
			if (actionSpace.size() == 1)
			{
				applyActionToGameState(forwardModel, gameState, actionSpace, actionSpace.at(0));
				simulatedTurns++;
			}
			else
			{
				applyActionToGameState(forwardModel, gameState, actionSpace, GetPortfolioAction(gameState, actionSpace, playerPortfolios, opponentPortfolios));
			}
		}
		
		return params_.heuristic->evaluateGameState(forwardModel, gameState, playerID);
	}

	// returns the action defined by the player's portfolio
	Action PortfolioGreedySearchAgent::GetPortfolioAction(GameState& gameState, std::vector<SGA::Action>& actionSpace, std::map<int, BaseActionScript*>& portfolioMap1, std::map<int, BaseActionScript*>& portfolioMap2)
	{
		//todo check if it is an entity action, search for the first action of an entity
		const int nextUnit = actionSpace.at(0).getSourceID();
		const int ownerID = gameState.getEntityConst(nextUnit)->ownerID;
		
		if (portfolioMap1.contains(nextUnit))
		{
			return portfolioMap1[nextUnit]->getActionForUnit(gameState, actionSpace, ownerID, nextUnit);
		}
		if (portfolioMap2.contains(nextUnit))
		{
			
			portfolioMap2[nextUnit]->getActionForUnit(gameState, actionSpace, ownerID, nextUnit);
		}
		
		return params_.portfolio[rand() % params_.portfolio.size()].get()->getActionForUnit(gameState, actionSpace, ownerID, nextUnit);
	}

	// applies an action to the current game state, reduces the number of remaining forward model calls and updates the action space
	void PortfolioGreedySearchAgent::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action)
	{
		params_.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(gameState, action);
		actionSpace = forwardModel.generateActions(gameState, gameState.getCurrentTBSPlayer());
	}
}
