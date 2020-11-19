#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{
	void TBSForwardModel::advanceGameState(TBSGameState& state, const TBSAction& action) const
	{
		FMState myState(state);
		bool canExecuteAction = action.validate(state);
		if(canExecuteAction)
		{
			switch (action.type)
			{
				case TBSActionType::Attack: executeAttack(myState, action); break;
				case TBSActionType::Move: executeMove(myState, action); break;
				case TBSActionType::Heal: executeHeal(myState, action); break;
				case TBSActionType::Push: executePush(myState, action); break;
				case TBSActionType::EndTurn: executeEndOfTurn(myState, action); break;
				default: throw std::runtime_error("Tried executing an action with an not supported action-type");
			}
		}

		TBSUnit* sourceUnit = state.getUnit(action.sourceUnitID);
		if (canExecuteAction && sourceUnit != nullptr)
		{
			sourceUnit->executedActionTypes.insert(action.type);
			sourceUnit->numActionsExecuted++;
		}

		// Remove all units that were killed
		for(auto id : myState.deadUnitIDs)
		{
			state.removeUnit(id);
		}

		state.isGameOver = checkGameIsFinished(state);
	}

	void TBSForwardModel::advanceGameState(TBSGameState& state, const TBSAction& action, std::vector<SGA::TBSAction>& actionSpace) const
	{
		//advance the gamestate
		advanceGameState(state, action);

		//Fake update of actionSpace
		//TODO update action space depending of the action played
		actionSpace.clear();
		actionSpace = generateActions(state);
	}

	bool TBSForwardModel::isValid(TBSGameState& state, const TBSAction& action) const
	{
		return action.validate(state);
	}

	std::vector<TBSAction> TBSForwardModel::generateActions(TBSGameState& state) const
	{
		return actionSpace->generateActions(state);
	}

	std::vector<TBSAction> TBSForwardModel::generateActions(TBSGameState& state, int playerID) const
	{
		return actionSpace->generateActions(state, playerID);
	}

	void TBSForwardModel::executeMove(FMState& state, const TBSAction& action) const
	{
		moveUnit(state, *state.target.getUnit(action.sourceUnitID), action.targetPosition);
	}

	void TBSForwardModel::executeAttack(FMState& state, const TBSAction& action) const
	{
		auto* attacker = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);
		damageUnit(state, *target, attacker->getAttackDamage());
	}

	void TBSForwardModel::executePush(FMState& state, const TBSAction& action) const
	{
		auto* pusher = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);
		auto pushDir = target->getPosition() - pusher->getPosition();
		auto newTargetPos = target->getPosition() + pushDir;
		if (state.target.isInBounds(newTargetPos) && state.target.isWalkable(newTargetPos))
		{
			moveUnit(state, *target, newTargetPos);
		}
	}

	void TBSForwardModel::executeHeal(FMState& state, const TBSAction& action) const
	{
		auto* healer = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);

		int& targetHealth = target->getHealth();
		targetHealth += healer->getHealAmount();
	}

	void TBSForwardModel::executeEndOfTurn(FMState& state, const TBSAction& action) const
	{
		endTurn(state);
	}

	// --- Start: Utility methods for game logic ---
	bool TBSForwardModel::canKill(const TBSGameState& state, Vector2i pos) const
	{
		auto targetTile = state.getBoard().getTile(pos.x, pos.y);
		if (!targetTile.isWalkable)
			return false;

		for (auto& effect : onTileEnterEffects)
		{
			if (effect.type == EffectType::Death && effect.targetTileTypeID == targetTile.tileTypeID)
				return true;
		}

		return false;
	}
	
	void TBSForwardModel::endTurn(FMState& state) const
	{
		executeEndOfTurnTrigger(state);

		// Find the next player who's still able to play
		for(auto i = 1; i <= state.target.getPlayerNumber(); i++)
		{
			int nextPlayerID = (state.target.currentPlayer + i) % state.target.getPlayerNumber();
			auto* targetPlayer = state.target.getPlayer(nextPlayerID);
			
			if (nextPlayerID == 0)
			{
				state.target.currentGameTurn++;
			}
			
			if(targetPlayer->canPlay)
			{
				state.target.currentPlayer = nextPlayerID;
				break;
			}
		}
		
		initTurn(state);
	}

	void TBSForwardModel::initTurn(FMState& state) const
	{
		for (TBSUnit& unit : state.target.getUnits())
		{
			unit.numActionsExecuted = 0;
			unit.executedActionTypes.clear();
		}
	}
	
	bool TBSForwardModel::checkGameIsFinished(TBSGameState& state) const
	{
		if (state.currentGameTurn >= state.roundLimit)
			return true;
		
		int numberPlayerCanPlay=0;
		int winnerID=-1;
		for (TBSPlayer& player : state.getPlayers())
		{
			if (player.canPlay && canPlayerPlay(player))
			{
				winnerID = player.playerID;
				numberPlayerCanPlay++;
			}
			else
			{
				player.canPlay = false;
			}
		}
		
		if(numberPlayerCanPlay <= 1)
		{
			state.setWinnerID(winnerID);
			return true;
		}
		
		return false;
	}

	void TBSForwardModel::moveUnit(FMState& state, TBSUnit& u, Vector2i newPosition) const
	{;
		u.setPosition(newPosition);
		executeOnEnterTileTrigger(state, u);
	}

	void TBSForwardModel::killUnit(FMState& state, TBSUnit& u) const
	{
		state.deadUnitIDs.emplace_back(u.getUnitID());
	}
	
	void TBSForwardModel::damageUnit(FMState& state, TBSUnit& u, int damageAmount) const
	{
		int& targetHealth = u.getHealth();
		targetHealth -= damageAmount;
		if (u.isDead())
		{
			killUnit(state, u);
		}
	}

	bool TBSForwardModel::canPlayerPlay(TBSPlayer& player) const
	{

		if (player.state.get().fogOfWarId != -1 && player.playerID != player.state.get().fogOfWarId)
			return true;

		switch (winCondition)
		{
			case WinConditionType::UnitAlive:
			{
				bool hasKing = false;
				std::vector<TBSUnit*> units = player.getUnits();

				for (auto& unit : units)
				{
					//Check if player has units
					if (unit->getUnitTypeID() == unitTypeID)
					{
						hasKing = true;
					}
				}

				if (!hasKing)
				{
					return false;
				}

				break;
			}
			case WinConditionType::LastManStanding:
			{
				std::vector<TBSUnit*> units = player.getUnits();

				if (units.empty())
				{
					return false;
				}
				break;
			}
		}

		return true;

	}
	// --- End: Utility methods for game logic ---

	// --- Start: Effect handling ---
	void TBSForwardModel::executeEndOfTurnTrigger(FMState& state) const
	{
		// Important! Do not swap for-loops, we want to execute an effect on all units first
		for(auto& effect : unitEndOfTurnEffects)
		{
			for(auto& unit : state.target.getUnits())
			{
				executeEffect(state, effect, unit);
			}
		}
	}
	
	void TBSForwardModel::executeOnEnterTileTrigger(FMState& state, TBSUnit& targetUnit) const
	{
		for(auto& effect : onTileEnterEffects)
		{
			executeEffect(state, effect, targetUnit);
		}
	}

	bool TBSForwardModel::isConditionFulfilled(const Effect& effect, TBSUnit& targetUnit) const
	{
		switch (effect.conditionType)
		{
			case ConditionType::None: return true;
			case ConditionType::StandingOnTile: return targetUnit.state.get().getBoard().getTile(targetUnit.getPosition().x, targetUnit.getPosition().y).tileTypeID == effect.targetTileTypeID;
			default: throw std::runtime_error("Tried validating an unknown effect condition");
		}
	}
	
	void TBSForwardModel::executeEffect(FMState& state, const Effect& effect, TBSUnit& targetUnit) const
	{
		if (!isConditionFulfilled(effect, targetUnit))
			return;

		switch (effect.type)
		{
			case EffectType::Death: killUnit(state, targetUnit); break;
			case EffectType::Damage: damageUnit(state, targetUnit, effect.damage); break;
			default: throw std::runtime_error("Tried executing an unknown effect type");
		}
	}
	
	void TBSForwardModel::addOnTileEnterEffect(Effect&& effect)
	{
		onTileEnterEffects.emplace_back(effect);
	}
	
	void TBSForwardModel::addUnitEndOfTurnEffect(Effect&& effect)
	{
		unitEndOfTurnEffects.emplace_back(effect);
	}
	// --- End: Effect handling ---
}
