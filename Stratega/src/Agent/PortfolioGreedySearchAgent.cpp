#include <Stratega/Agent/PortfolioGreedySearchAgent.h>


namespace SGA
{
	ActionAssignment PortfolioGreedySearchAgent::computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs)
	{
		auto actionSpace = forwardModel->generateActions(state, getPlayerID());

		if (actionSpace.size() == 1)
		{
			return ActionAssignment::fromSingleAction(actionSpace.at(0));
		}
		else
		{
			params_.REMAINING_FM_CALLS = params_.MAX_FM_CALLS;
					
			// retrieving unit sets
			std::map<int, std::vector<Entity*>> unitsPerPlayer = std::map<int, std::vector<Entity*>>();
			for (auto player : state.players)
			{
				unitsPerPlayer[player.id] = state.getPlayerEntities(player.id);
			}

			// sets a random portfolio for each unit per player
			std::map<int, BaseActionScript*> unitScriptAssignments;
			InitializePortfolios(unitsPerPlayer, unitScriptAssignments);

			
			// improve the portfolio until the number of available forward model calls has been used
			while (params_.REMAINING_FM_CALLS > 0)
			{
				// first improve the script assignment of all opponents:
				for (auto opponent : state.players)
				{
					if (opponent.id != getPlayerID())
						Improve(*forwardModel, state, unitScriptAssignments, opponent.id);
				}
			
				Improve(*forwardModel, state, unitScriptAssignments, getPlayerID());
			}

			// return the first action of the player's portfolio
			auto action = GetPortfolioAction(state, actionSpace, unitScriptAssignments);

			return ActionAssignment::fromSingleAction(action);
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
	void PortfolioGreedySearchAgent::InitializePortfolios(std::map<int, std::vector<Entity*>>& unitsPerPlayer, std::map<int, BaseActionScript*>& unitScriptAssignments)
	{
		for (auto playerEntry : unitsPerPlayer)
		{
			for (Entity* u : playerEntry.second)
			{
				unitScriptAssignments[u->id] = params_.portfolio[rand() % params_.portfolio.size()].get();
			}
		}
	}

	void PortfolioGreedySearchAgent::Improve(const ForwardModel& forwardModel, GameState& gameState, std::map<int, BaseActionScript*>& unitScriptAssignments, int playerID)
	{		
		double currentPortfolioValue = Playout(forwardModel, gameState, unitScriptAssignments, playerID);
		std::vector<Entity*> playerEntities = gameState.getPlayerEntities(playerID);
		
		for (int i = 0; i < params_.ITERATIONS_PER_IMPROVE; i++)
		{
			for (auto unit : playerEntities)
			{
				int unitId = unit->id;
				BaseActionScript* previousBestScript = unitScriptAssignments[unitId];
				double bestNewScriptValue = -std::numeric_limits<double>::max();
				int bestScriptIndex = -1;

				// search for best script for the current unit
				for (int j = 0; j < params_.portfolio.size(); j++)
				{
					//if (params_.PORTFOLIO[i].get() == previousBestScript) 
					//	continue;	// skip the playout from the previousBestScript
					
					unitScriptAssignments[unitId] = params_.portfolio[j].get();
					const double currentScriptValue = Playout(forwardModel, gameState, unitScriptAssignments, playerID);
					if (currentScriptValue > bestNewScriptValue)
					{
						bestScriptIndex = j;
						bestNewScriptValue = currentScriptValue;
					} 
				}

				// update portfolio and its value
				if (bestNewScriptValue > currentPortfolioValue)
				{
					// in case we found a better script, permanently replace the old one
					unitScriptAssignments[unitId] = params_.portfolio[bestScriptIndex].get();
					currentPortfolioValue = bestNewScriptValue;
				}
				else
				{
					// if no script has been better than the previous best one, we just reset the portfolio
					unitScriptAssignments[unitId] = previousBestScript;
				}

				//if (params_.REMAINING_FM_CALLS <= 0)
				//	return;
			}
		}
	}

	// Simulated the players PORTFOLIO until a maximum number of turns has been simulated or the game has ended
	// returns the heuristic value of the simulated game state
	double PortfolioGreedySearchAgent::Playout(const ForwardModel& forwardModel, GameState gameState, std::map<int, BaseActionScript*>& unitScriptAssignments, int playerID)
	{
		int initialTick = gameState.currentTick;
		auto actionSpace = forwardModel.generateActions(gameState, gameState.getCurrentTBSPlayer());
		
		while (gameState.currentTick - initialTick < params_.NR_OF_TICKS_PLANNED && !gameState.isGameOver && actionSpace.size() != 0)
		{
			applyActionToGameState(forwardModel, gameState, actionSpace, GetPortfolioAction(gameState, actionSpace, unitScriptAssignments), unitScriptAssignments, playerID);	
		}
		
		return params_.heuristic->evaluateGameState(forwardModel, gameState, playerID);
	}

	// returns the action defined by the player's portfolio
	Action PortfolioGreedySearchAgent::GetPortfolioAction(GameState& gameState, std::vector<SGA::Action>& actionSpace, const std::map<int, BaseActionScript*>& unitScriptAssignments)
	{
		if (actionSpace.size() == 1)
		{
			return actionSpace[0];
		}
		
		//todo check if it is an entity action, search for the first action of an entity
		const int nextUnit = actionSpace.at(0).getSourceID();
		const int ownerID = gameState.getEntityConst(nextUnit)->ownerID;
		
		if (unitScriptAssignments.contains(nextUnit))
		{
			auto action = unitScriptAssignments.at(nextUnit)->getActionForUnit(gameState, actionSpace, ownerID, nextUnit);

			// some scripts immediately try to end the turn in case they cannot find a suitable action
			// check the other units instead
			if (action.actionTypeFlags == ActionFlag::EndTickAction)
			{
				// check if there is another unit that would be able to do something
				for (auto entity : gameState.getPlayerEntities(ownerID))
				{
					if (entity->id != nextUnit)
					{
						action = unitScriptAssignments.at(entity->id)->getActionForUnit(gameState, actionSpace, ownerID, entity->id);
						if (action.actionTypeFlags != ActionFlag::EndTickAction)
						{
							return action;
						}
					}
				}
			}

			// only end the turn in case all unit script assignments want to end it
			return action;
		}

		// there might be something wrong here, return a random action instead
		const std::uniform_int_distribution distribution(0, static_cast<int>(params_.portfolio.size() - 1));
		return params_.portfolio[distribution(getRNGEngine())].get()->getActionForUnit(gameState, actionSpace, ownerID, nextUnit);
	}

	// applies an action to the current game state, reduces the number of remaining forward model calls and updates the action space
	void PortfolioGreedySearchAgent::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action, std::map<int, BaseActionScript*>& unitScriptAssignments, int playerID)
	{
		//Roll the game state with our action.
		params_.REMAINING_FM_CALLS -= PortfolioGreedySearchAgent::roll(gameState, forwardModel, action, playerID, unitScriptAssignments);

		//Continue rolling the state until the game is over, we run out of budget or this agent can play again. 
		while (!gameState.canPlay(params_.PLAYER_ID) && params_.REMAINING_FM_CALLS > 0 && !gameState.isGameOver)
		{
			//Roll actions for the opponent(s).
			params_.REMAINING_FM_CALLS -= PortfolioGreedySearchAgent::rollOppOnly(gameState, forwardModel, unitScriptAssignments);
		}

		//params_.REMAINING_FM_CALLS--;
		//forwardModel.advanceGameState(gameState, action);
		actionSpace = forwardModel.generateActions(gameState, gameState.getCurrentTBSPlayer());
	}


	/// <summary>
	/// Rolls the state forward for all players in the game. For the player with an ID == agParams.PLAYER_ID, the action 'act' will be executed. 
	/// For the rest, the opponent model from agParams is used to decide the action. If no opponent model is defined, nothing / skip turn is executed.
	/// </summary>
	/// <param name="gs">Game state to roll forward</param>
	/// <param name="fm">Forward model to roll the state forward and quering available actions.</param>
	/// <param name="act">Action to execute for the player with id equals to agParams.PLAYER_ID</param>
	/// <param name="agParams">Parameters of the agent who's calling this method.</param>
	/// <returns>The number of actions executed by this function.</returns>
	inline int PortfolioGreedySearchAgent::roll(GameState& gs, const ForwardModel& fm, const Action& act, int playerID, const std::map<int, BaseActionScript*>& unitScriptAssignments)
	{
		int actionsExecuted = 0;
		std::vector<int> whoCanPlay = gs.whoCanPlay();

		for (int id : whoCanPlay)
		{
			if (id == playerID)
			{
				//This is the player for which we passed the action to execute.
				fm.advanceGameState(gs, act);
				actionsExecuted++;
			}
			//An opponent is to play in this state, execute:
			else if (playerID != id)
			{
				rollOppAction(gs, fm, unitScriptAssignments, id);
				actionsExecuted++;
			}
		}
		return actionsExecuted;
	}

	inline int PortfolioGreedySearchAgent::rollOppOnly(GameState& gs, const ForwardModel& fm, const std::map<int, BaseActionScript*>& unitScriptAssignments)
	{
		int actionsExecuted = 0;
		std::vector<int> canPlay = gs.whoCanPlay();
		for (int id : canPlay)
		{
			//Try to execute an action for this player if it's an opponent opponent
			if (id != getPlayerID() && rollOppAction(gs, fm, unitScriptAssignments, id))  actionsExecuted++;
		}
		return actionsExecuted;
	}

	/// <summary>
	/// Given an opponent ID, it rolls the state forward executing an action dictated by the opponent model from agParams.
	/// If no opponent model is set, nothing / skip turn is executed.
	/// </summary>
	/// <param name="gs">Game state to roll forward</param>
	/// <param name="fm">Forward model to roll the state forward and quering available actions.</param>
	/// <param name="agParams">Parameters of the agent who's calling this method.</param>
	/// <param name="oppID">ID of the opponent who will roll the state forward with an action.</param>
	/// <returns>True if one action was executed.</returns>
	inline bool PortfolioGreedySearchAgent::rollOppAction(GameState& gs, const ForwardModel& fm, const std::map<int, BaseActionScript*>& unitScriptAssignments, int oppID)
	{
		//An opponent is to play in this state.
		auto actions = fm.generateActions(gs, oppID);
		fm.advanceGameState(gs, GetPortfolioAction(gs, actions, unitScriptAssignments));
		return true;
	}
	
}
