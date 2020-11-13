#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{
	void TBSForwardModel::advanceGameState(TBSGameState& state, const TBSAction& action) const
	{
		FMState myState(state);
		bool executedAction;
		switch (action.type)
		{
			case TBSActionType::Attack: executedAction = executeAttack(myState, action); break;
			case TBSActionType::Move: executedAction = executeMove(myState, action); break;
			case TBSActionType::Heal: executedAction = executeHeal(myState, action); break;
			case TBSActionType::Push: executedAction = executePush(myState, action); break;
			case TBSActionType::EndTurn: executedAction = executeEndOfTurn(myState, action); break;
			default: throw std::runtime_error("Tried executing an action with an not supported action-type");
		}

		TBSUnit* sourceUnit = state.getUnit(action.sourceUnitID);
		if (executedAction && sourceUnit != nullptr)
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
		actionSpace = getActions(state);
	}

	bool TBSForwardModel::isValid(TBSGameState& state, const TBSAction& action) const
	{
		switch (action.type)
		{
			case TBSActionType::Attack: return validateAttack(state, action);
			case TBSActionType::Move: return validateMove(state, action);
			case TBSActionType::Heal: return validateHeal(state, action);
			case TBSActionType::Push: return validatePush(state, action);
			case TBSActionType::EndTurn: return validateEndOfTurn(state, action);
			default: throw std::runtime_error("Tried validating an action with an not supported action-type");
		}
	}

	std::vector<TBSAction> TBSForwardModel::getActions(TBSGameState& state) const
	{
		return getActions(state, state.currentPlayer);
	}

	std::vector<TBSAction> TBSForwardModel::getActions(TBSGameState& state, int playerID) const
	{
		std::vector<TBSAction> actions;
		for(auto* unit : state.getPlayer(playerID)->getUnits())
		{
			if(unit->numActionsExecuted >= unit->getType().actionsPerTurn)
			{
				continue;
			}
			
			for(auto actionType : unit->getType().tbsActions)
			{
				if(!unit->getType().canRepeatActions && unit->executedActionTypes.find(actionType) != unit->executedActionTypes.end())
				{
					continue;
				}

				switch (actionType)
				{
					case TBSActionType::Attack: generateAttackActions(*unit, actions); break;
					case TBSActionType::Move: generateMoveActions(*unit, actions); break;
					case TBSActionType::Heal: generateHealActions(*unit, actions); break;
					case TBSActionType::Push: generatePushActions(*unit, actions); break;
					default: throw std::runtime_error("Unit can execute an invalid action-type");
				}
			}
		}

		generateEndOfTurnActions(state, playerID, actions);
		return actions;
	}

	void TBSForwardModel::generateMoveActions(TBSUnit& unit, std::vector<SGA::TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		auto moveRange = unit.getRange();
		auto startCheckPositionX = std::max(0, unit.getPosition().x - moveRange);
		auto endCheckPositionX = std::min(state.getBoard().getWidth() - 1, unit.getPosition().x + moveRange);
		auto startCheckPositionY = std::max(0, unit.getPosition().y - moveRange);
		auto endCheckPositionY = std::min(state.getBoard().getHeight() - 1, unit.getPosition().y + moveRange);
		for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
		{
			for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
			{
				TBSAction action{ TBSActionType::Move, unit.getPlayerID(), unit.getUnitID(), SGA::Vector2i(x, y), -1};
				if(validateMove(state, action))
				{
					actionBucket.emplace_back(action);
				}
			}
		}
	}
	
	void TBSForwardModel::generateAttackActions(TBSUnit& unit, std::vector<SGA::TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (const auto& targetUnit : state.getUnits())
		{
			TBSAction attackAction{ TBSActionType::Attack, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if (validateAttack(state, attackAction))
			{
				actionBucket.emplace_back(attackAction);
			}
		}
	}
	
	void TBSForwardModel::generatePushActions(TBSUnit& unit, std::vector<SGA::TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (const auto& targetUnit : state.getUnits())
		{
			TBSAction pushAction{ TBSActionType::Push, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if (validatePush(state, pushAction))
			{
				actionBucket.emplace_back(pushAction);
			}
		}
	}
	
	void TBSForwardModel::generateHealActions(TBSUnit& unit, std::vector<SGA::TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for(const auto& targetUnit : state.getUnits())
		{
			TBSAction healAction{ TBSActionType::Heal, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if(validateHeal(state, healAction))
			{
				actionBucket.emplace_back(healAction);
			}
		}
	}
	
	void TBSForwardModel::generateEndOfTurnActions(TBSGameState& state, int playerID, std::vector<TBSAction>& actionBucket) const
	{
		TBSAction endTurnAction{ TBSActionType::EndTurn, playerID, -1, {}, -1 };
		if(validateEndOfTurn(state, endTurnAction))
		{
			actionBucket.emplace_back(endTurnAction);
		}
	}

	bool TBSForwardModel::executeMove(FMState& state, const TBSAction& action) const
	{
		if (!validateMove(state.target, action))
			return false;

		moveUnit(state, *state.target.getUnit(action.sourceUnitID), action.targetPosition);
		return true;
	}

	bool TBSForwardModel::executeAttack(FMState& state, const TBSAction& action) const
	{
		if (!validateAttack(state.target, action))
			return false;

		auto* attacker = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);
		damageUnit(state, *target, attacker->getAttackDamage());
		return true;
	}

	bool TBSForwardModel::executePush(FMState& state, const TBSAction& action) const
	{
		if (!validatePush(state.target, action))
			return false;

		auto* pusher = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);
		auto pushDir = target->getPosition() - pusher->getPosition();
		auto newTargetPos = target->getPosition() + pushDir;
		if (state.target.isInBounds(newTargetPos) && state.target.isWalkable(newTargetPos))
		{
			moveUnit(state, *target, newTargetPos);
		}
		return true;
	}

	bool TBSForwardModel::executeHeal(FMState& state, const TBSAction& action) const
	{
		if (!validateHeal(state.target, action))
			return false;

		auto* healer = state.target.getUnit(action.sourceUnitID);
		auto* target = state.target.getUnit(action.targetUnitID);

		int& targetHealth = target->getHealth();
		targetHealth += healer->getHealAmount();
		
		return true;
	}

	bool TBSForwardModel::executeEndOfTurn(FMState& state, const TBSAction& action) const
	{
		if (!validateEndOfTurn(state.target, action))
			return false;
		endTurn(state);
		return true;
	}

	bool TBSForwardModel::validateMove(TBSGameState& state, const TBSAction& action) const
	{
		TBSUnit* unit = state.getUnit(action.sourceUnitID);
		if (unit == nullptr)
			return false;
		
		return state.isWalkable(action.targetPosition) && unit->getPosition().manhattanDistance(action.targetPosition) <= unit->getRange();
	}

	bool TBSForwardModel::validateAttack(TBSGameState& state, const TBSAction& action) const
	{
		auto* attacker = state.getUnit(action.sourceUnitID);
		auto* target = state.getUnit(action.targetUnitID);
		if (attacker == nullptr || target == nullptr)
			return false;
		
		return attacker->getUnitID() != target->getUnitID() &&
			attacker->getPosition().manhattanDistance(target->getPosition()) <= attacker->getType().actionRange;
	}

	bool TBSForwardModel::validatePush(TBSGameState& state, const TBSAction& action) const
	{
		auto* pusher = state.getUnit(action.sourceUnitID);
		auto* target = state.getUnit(action.targetUnitID);
		if (pusher == nullptr || target == nullptr)
			return false;
		
		// We can only push in cardinal directions
		return pusher->getUnitID() != target->getUnitID() && 
			pusher->getPosition().manhattanDistance(target->getPosition()) == 1;
	}

	bool TBSForwardModel::validateHeal(TBSGameState& state, const TBSAction& action) const
	{
		auto* healer = state.getUnit(action.sourceUnitID);
		auto* target = state.getUnit(action.targetUnitID);
		if (healer == nullptr || target == nullptr)
			return false;
		
		return target->getHealth() < target->getType().maxHealth && healer->getPosition().manhattanDistance(target->getPosition()) <= healer->getType().actionRange;
	}

	bool TBSForwardModel::validateEndOfTurn(TBSGameState& /*state*/, const TBSAction& /*action*/) const
	{
		return true;
	}

	// --- Start: Utility methods for game logic ---
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
