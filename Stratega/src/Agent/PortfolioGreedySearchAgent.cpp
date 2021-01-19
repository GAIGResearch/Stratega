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
				auto actionSpace = forwardModel.generateActions(gameState);

				if (actionSpace.size() == 1)
				{
					gameCommunicator.executeAction(actionSpace.at(0));
				}
				else
				{
					params_.REMAINING_FM_CALLS = params_.MAX_FM_CALLS;
					
					// player PORTFOLIO
					auto playerUnits = gameState.getPlayer(gameCommunicator.getPlayerID())->getUnits();
					std::map<int, BaseActionScript*> playerPortfolios = std::map<int, BaseActionScript*>();

					// opponent PORTFOLIO
					auto opUnits = gameState.getPlayer(1 - gameCommunicator.getPlayerID() % 2)->getUnits();
					std::map<int, BaseActionScript*> opponentPortfolios = std::map<int, BaseActionScript*>();

					// sets a random portfolio for each player
					InitializePortfolios(playerUnits, playerPortfolios);
					InitializePortfolios(opUnits, opponentPortfolios);

					// improve the portfolio until the number of available forward model calls has been used
					while (params_.REMAINING_FM_CALLS > 0)
					{
						Improve(forwardModel, gameState, opUnits, opponentPortfolios, playerPortfolios);
						if (params_.REMAINING_FM_CALLS <= 0)
							break;

						Improve(forwardModel, gameState, playerUnits, playerPortfolios, opponentPortfolios);
					}
					
					// return the first action of the player's portfolio
					gameCommunicator.executeAction(GetPortfolioAction(gameState, actionSpace, playerPortfolios, opponentPortfolios));
				}
			}
		}
	}
	
	// initialize the player's portfolio map with random scripts
	void PortfolioGreedySearchAgent::InitializePortfolios(std::vector<TBSUnit*>& units, std::map<int, BaseActionScript*>& portfolioMap)
	{
		for (TBSUnit* u : units)
		{
			portfolioMap[u->getUnitID()] = params_.PORTFOLIO[rand() % params_.PORTFOLIO.size()].get();
		}
	}

	void PortfolioGreedySearchAgent::Improve(TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<TBSUnit*>& playerUnits, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios)
	{		
		double currentPortfolioValue = Playout(forwardModel, gameState, playerPortfolios, opponentPortfolios);

		for (int i = 0; i < params_.ITERATIONS_PER_IMPROVE; i++)
		{
			for (auto unit : playerUnits)
			{
				int unitId = unit->getUnitID();
				BaseActionScript* previousBestScript = playerPortfolios[unitId];
				double bestNewScriptValue = -std::numeric_limits<double>::max();
				int bestScriptIndex = -1;

				// search for best script for the current unit
				for (int i = 0; i < params_.PORTFOLIO.size(); i++)
				{
					//if (params_.PORTFOLIO[i].get() == previousBestScript) 
					//	continue;	// skip the playout from the previousBestScript
					
					playerPortfolios[unitId] = params_.PORTFOLIO[i].get();
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
					playerPortfolios[unitId] = params_.PORTFOLIO[bestScriptIndex].get();
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
	double PortfolioGreedySearchAgent::Playout(TBSForwardModel& forwardModel, TBSGameState gameState, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios)
	{
		const int playerID = gameState.currentPlayer;
		auto actionSpace = forwardModel.generateActions(gameState);
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
		
		return params_.HEURISTIC.evaluateGameState(forwardModel, gameState, playerID);
	}

	// returns the action defined by the player's portfolio
	TBSAction PortfolioGreedySearchAgent::GetPortfolioAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, std::map<int, BaseActionScript*>& portfolioMap1, std::map<int, BaseActionScript*>& portfolioMap2)
	{
		const int nextUnit = actionSpace.at(0).sourceUnitID;
		if (portfolioMap1.contains(nextUnit))
		{
			return portfolioMap1[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
		}
		return portfolioMap2[nextUnit]->getActionForUnit(gameState, actionSpace, nextUnit);
	}

	// applies an action to the current game state, reduces the number of remaining forward model calls and updates the action space
	void PortfolioGreedySearchAgent::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, const TBSAction& action)
	{
		params_.REMAINING_FM_CALLS--;
		forwardModel.advanceGameState(gameState, action);
		actionSpace = forwardModel.generateActions(gameState);
	}
}
