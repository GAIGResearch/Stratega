#include <Agent/PortfolioGreedySearchAgent.h>

namespace SGA
{
	//todo only works for 2 players
	void PortfolioGreedySearchAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.getGameState().isGameOver)
		{
			if (gameCommunicator.isMyTurn())
			{
				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				auto actionSpace = forwardModel.getActions(gameState);

				if (actionSpace->count() == 1)
				{
					gameCommunicator.executeAction(actionSpace->getAction(0));
				}
				else
				{
					// player portfolios
					auto playerUnits = gameState.getPlayer(gameCommunicator.getPlayerID())->getUnits();
					std::map<int, BasePortfolio*> playerPortfolios = std::map<int, BasePortfolio*>();

					// opponent portfolios
					auto opUnits = gameState.getPlayer(1 - gameCommunicator.getPlayerID() % 2)->getUnits();
					std::map<int, BasePortfolio*> opponentPortfolios = std::map<int, BasePortfolio*>();

					// sets a random portfolio for each player
					InitializePortfolios(playerUnits, playerPortfolios);
					InitializePortfolios(opUnits, opponentPortfolios);

					// improve the portfolio until the number of available forward model calls has been used
					while (params_.REMAINING_FM_CALLS > 0)
					{
						Improve(forwardModel, gameState, opUnits, opponentPortfolios, playerPortfolios);
						Improve(forwardModel, gameState, playerUnits, playerPortfolios, opponentPortfolios);
					}
					
					// return the first action of the player's portfolio
					gameCommunicator.executeAction(GetPortfolioAction(gameState, actionSpace, playerPortfolios, opponentPortfolios));
				}
			}
		}
	}
	
	// initialize the player's portfolio map with random scripts
	void PortfolioGreedySearchAgent::InitializePortfolios(std::vector<TBSUnit*>& units, std::map<int, BasePortfolio*>& portfolioMap)
	{
		for (TBSUnit* u : units)
		{
			portfolioMap[u->getUnitID()] = params_.portfolios[rand() % params_.portfolios.size()].get();
		}
	}

	void PortfolioGreedySearchAgent::Improve(TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<TBSUnit*>& playerUnits, std::map<int, BasePortfolio*>& playerPortfolios, std::map<int, BasePortfolio*>& opponentPortfolios)
	{		
		double currentPortfolioValue = Playout(forwardModel, gameState, playerPortfolios, opponentPortfolios);

		for (int i = 0; i < params_.ITERATIONS_PER_IMPROVE; i++)
		{
			for (auto unit : playerUnits)
			{
				int unitId = unit->getUnitID();
				BasePortfolio* previousBestScript = playerPortfolios[unitId];
				double bestNewScriptValue = -std::numeric_limits<double>::max();
				int bestScriptIndex = -1;

				// search for best script for the current unit
				for (int i = 0; i < params_.portfolios.size(); i++)
				{
					if (params_.portfolios[i].get() == previousBestScript) 
						continue;	// skip the playout from the previousBestScript
					
					playerPortfolios[unitId] = params_.portfolios[i].get();
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
					playerPortfolios[unitId] = params_.portfolios[bestScriptIndex].get();
					currentPortfolioValue = bestNewScriptValue;
				}
				else
				{
					// if no script has been better than the previous best one, we just reset the portfolio
					playerPortfolios[unitId] = previousBestScript;
				}
			}
		}
	}

	// Simulated the players portfolios until a maximum number of turns has been simulated or the game has ended
	// returns the heuristic value of the simulated game state
	double PortfolioGreedySearchAgent::Playout(TBSForwardModel& forwardModel, TBSGameState gameState, std::map<int, BasePortfolio*>& playerPortfolios, std::map<int, BasePortfolio*>& opponentPortfolios)
	{
		const int playerID = gameState.currentPlayer;
		auto actionSpace = forwardModel.getActions(gameState);
		int simulatedTurns = 0;
		while (simulatedTurns < params_.NR_OF_TURNS_PLANNED && !gameState.isGameOver && actionSpace->count()!=0)
		{
			if (actionSpace->count() == 1)
			{
				applyActionToGameState(forwardModel, gameState, actionSpace, actionSpace->getAction(0));
				simulatedTurns++;
			}
			else
			{
				applyActionToGameState(forwardModel, gameState, actionSpace, GetPortfolioAction(gameState, actionSpace, playerPortfolios, opponentPortfolios));
			}
		}
		
		return params_.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
	}

	// returns the action defined by the player's portfolio
	Action<Vector2i> PortfolioGreedySearchAgent::GetPortfolioAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, std::map<int, BasePortfolio*>& portfolioMap1, std::map<int, BasePortfolio*>& portfolioMap2)
	{
		const int nextUnit = actionSpace->getAction(0).getSourceUnitID();
		if (portfolioMap1.contains(nextUnit))
		{
			return portfolioMap1[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
		}
		return portfolioMap2[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
	}

	// applies an action to the current game state, reduces the number of remaining forward model calls and updates the action space
	void PortfolioGreedySearchAgent::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, const Action<Vector2i>& action)
	{
		params_.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(gameState, action);
		actionSpace = forwardModel.getActions(gameState);
	}
}
