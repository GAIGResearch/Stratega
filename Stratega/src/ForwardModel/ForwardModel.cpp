#include <Stratega/ForwardModel/ForwardModel.h>

namespace SGA
{
	ForwardModel::ForwardModel()
		: actionSpace(generateDefaultActionSpace())
	{
	}
	
	std::vector<Action> ForwardModel::generateActions(const GameState& state, int playerID) const
	{
		std::vector<Action> actionBucket;
		generateActions(state, playerID, actionBucket);
		return actionBucket;
	}

	void ForwardModel::generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const
	{
		actionBucket = actionSpace->generateActions(state, playerID);
	}


	
	bool ForwardModel::checkPlayerLost(const GameState& state, int playerID) const
	{
		if (state.getFogOfWarTileId() != -1 && playerID != state.getFogOfWarTileId())
			return false;

		//Check Lose conditions
		std::vector<ActionTarget> targets;
		targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
		
		for (auto& loseConditionType : loseConditions)
		{
			for (auto& loseCondition : loseConditionType)
			{
				if (loseCondition->isFulfilled(state, targets))
					return true;
			}
		}

		return false;
	}

	bool ForwardModel::checkPlayerWon(const GameState& state, int playerID) const
	{
		if (state.getFogOfWarTileId() != -1 && playerID != state.getFogOfWarTileId())
			return false;

		//Check Win conditions
		std::vector<ActionTarget> targets;
		targets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));

		//Check win condition types
		for (auto& winConditionType : winConditions)
		{
			bool playerWon = true;
			
			//Check condition list
			for (auto& winCondition : winConditionType)
			{
				if (!winCondition->isFulfilled(state, targets))
				{
					playerWon = false;
					break;
				}
					
			}

			if (playerWon)
				return true;
		}

		return false;
	}


	void ForwardModel::executeAbortContinuousAction(GameState& state, const Action& action) const
	{
		if (action.targets[0].getType() == ActionTarget::PlayerReference)
		{
			auto& sourcePlayer = action.targets[0].getPlayer(state);
			int continuousActionID = action.targets[1].getContinuousActionID();

			//Search continuousAction to abort
			for (size_t i = 0; i < sourcePlayer.continuousAction.size(); i++)
			{
				if (sourcePlayer.continuousAction[i].continuousActionID == continuousActionID)
				{
					auto& actionType = sourcePlayer.continuousAction[i].getActionType();

					//Execute OnAbort Effects				
					for (auto& effect : actionType.OnAbort)
					{
						effect->execute(state, *this, sourcePlayer.continuousAction[i].targets);
					}

					//Remove continuous action
					sourcePlayer.continuousAction.erase(sourcePlayer.continuousAction.begin() + i);
					i--;
				}
			}
		}
		else
		{
			auto& sourceEntity = *action.targets[0].getEntity(state);
			int continuousActionID = action.targets[1].getContinuousActionID();

			//Search continuousAction to abort
			std::vector<Action>& continuousActions = sourceEntity.getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				if (continuousActions[i].continuousActionID == continuousActionID)
				{
					auto& actionType = continuousActions[i].getActionType();

					//Execute OnAbort Effects				
					for (auto& effect : actionType.OnAbort)
					{
						effect->execute(state, *this, continuousActions[i].targets);
					}

					//Remove continuous action
					continuousActions.erase(continuousActions.begin() + i);
					i--;
				}
			}
		}
	}

	void ForwardModel::executeContinuousAction(GameState& state, const Action& action) const
	{
		auto& actionType = action.getActionType();
		//If we are generating continuousAction we need to track them somehow
		//Using ID for each action for example				
		Action newAction = action;
		newAction.continuousActionID = state.getNextContinuousActionID();
		state.incNextContinuousActionID();
		newAction.targets.emplace_back(ActionTarget::createContinuousActionActionTarget(newAction.continuousActionID));

		//If is continues we execute OnStart Effects
		//and we add the action to the list of continuous actions
		if (actionType.sourceType == ActionSourceType::Entity)
		{
			auto& type = state.gameInfo->actionTypes->at(actionType.id);
			for (auto& effect : type.OnStart)
			{
				effect->execute(state, *this, newAction.targets);
			}

			auto& executingEntity = *newAction.targets[0].getEntity(state);
			executingEntity.getContinuousActions().emplace_back(newAction);
		}
		else if (actionType.sourceType == ActionSourceType::Player)
		{
			auto& type = state.gameInfo->actionTypes->at(actionType.id);
			for (auto& effect : type.OnStart)
			{
				effect->execute(state, *this, newAction.targets);
			}

			auto& executingPlayer = newAction.targets[0].getPlayer(state);
			executingPlayer.continuousAction.emplace_back(newAction);
		}
	}

	void ForwardModel::executeNormalAction(GameState& state, const Action& action) const
	{
		const auto& actionType = action.getActionType();
		if (actionType.sourceType == ActionSourceType::Entity)
		{
			auto& executingEntity = *action.targets[0].getEntity(state);
			executingEntity.setActionTicks(action.getActionTypeID(), state.currentTick);
		}
		else
		{
			// Remember when the action was executed
			auto& executingPlayer = action.targets[0].getPlayer(state);
			// ToDo We should probably find a way to avoid this loop
			for (auto& actionInfo : executingPlayer.attachedActions)
			{
				if (actionInfo.actionTypeID == action.getActionTypeID())
				{
					actionInfo.lastExecutedTick = state.currentTick;
					break;
				}
			}
		}

		action.execute(state, *this);
	}

	void ForwardModel::executeAction(GameState& state, const Action& action) const
	{
		switch (action.actionTypeFlags)
		{
		case ActionFlag::AbortContinuousAction: executeAbortContinuousAction(state, action); break;
		case ActionFlag::ContinuousAction: executeContinuousAction(state, action); break;
		case ActionFlag::EndTickAction: break;
		default: executeNormalAction(state, action);
		}
	}


	void ForwardModel::executeOnTriggerEffects(GameState& state) const
	{
		// Execute OnTick-trigger
		for (const auto& onTickEffect : onTickEffects)
		{
			for (const auto& entity : state.entities)
			{
				if (onTickEffect.validTargets.find(entity.getEntityTypeID()) == onTickEffect.validTargets.end())
					continue;
				std::vector<ActionTarget> targets;
				targets.emplace_back(ActionTarget::createEntityActionTarget(entity.id));
				auto isValid = true;
				for (const auto& condition : onTickEffect.conditions)
				{
					if (!condition->isFulfilled(state, targets))
					{
						isValid = false;
						break;
					}
				}

				if (isValid)
				{
					for (const auto& effect : onTickEffect.effects)
					{
						effect->execute(state, *this, targets);
					}
				}
			}
		}
	}

	void ForwardModel::checkEntitiesContinuousActionIsComplete(GameState& state) const
	{
		//Check if condition is complete
		for (size_t j = 0; j < state.entities.size(); j++)
		{
			std::vector<Action>& continuousActions = state.entities[j].getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				auto& actionType = continuousActions[i].getActionType();
				//Add one elapsed tick
				continuousActions[i].elapsedTicks++;
				//Execute OnTick Effects
				if (actionType.sourceType == ActionSourceType::Entity)
				{
					auto& type = state.gameInfo->actionTypes->at(actionType.id);
					for (auto& effect : type.OnTick)
					{
						effect->execute(state, *this, continuousActions[i].targets);
					}
				}
				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.triggerComplete)
				{
					if (!condition->isFulfilled(state, continuousActions[i].targets))
					{
						isComplete = false;
						break;
					}
				}

				if (isComplete)
				{
					//TODO Should the action check the targetconditions before executing it?
					//Check before we execute OnComplete Effects
					//if the conditions continue being true
					bool canExecute = true;
					/*for (auto& targetType : actionType.actionTargets)
					for (const auto& condition : targetType.second)
					{
						if (!condition->isFulfilled(state, state.entities[j].continuousAction[i].targets))
						{
							canExecute = false;
							break;
						}
					}*/

					if (canExecute)
					{
						//Execute OnComplete Effects
						if (actionType.sourceType == ActionSourceType::Entity)
						{
							auto& type = state.gameInfo->actionTypes->at(actionType.id);
							for (auto& effect : type.OnComplete)
							{
								effect->execute(state, *this, continuousActions[i].targets);
							}
						}
					}

					//Delete the ContinuousAction
					continuousActions.erase(continuousActions.begin() + i);
					i--;
					//Stop executing this action
					continue;
				}
			}
		}
	}

	void ForwardModel::checkPlayerContinuousActionIsComplete(GameState& state) const
	{
		//Player continuous Action
		for (size_t j = 0; j < state.players.size(); j++)
		{
			for (size_t i = 0; i < state.players[j].continuousAction.size(); i++)
			{
				auto& actionType = state.players[j].continuousAction[i].getActionType();
				//Add one elapsed tick
				state.players[j].continuousAction[i].elapsedTicks++;
				//Execute OnTick Effects
				if (actionType.sourceType == ActionSourceType::Player)
				{
					auto& type = state.gameInfo->actionTypes->at(actionType.id);
					for (auto& effect : type.OnTick)
					{
						effect->execute(state, *this, state.players[j].continuousAction[i].targets);
					}
				}

				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.triggerComplete)
				{
					if (!condition->isFulfilled(state, state.players[j].continuousAction[i].targets))
					{
						isComplete = false;
						break;
					}
				}

				if (isComplete)
				{
					//TODO Should the action check the targetconditions before executing it?
					//Check before we execute OnComplete Effects
					//if the conditions continue being true
					bool canExecute = true;
					/*for (auto& targetType : actionType.actionTargets)
					for (const auto& condition : targetType.second)
					{
						if (!condition->isFulfilled(state, state.players[j].continuousAction[i].targets))
						{
							canExecute = false;
							break;
						}
					}*/

					if (canExecute)
					{
						//Execute OnComplete Effects
						if (actionType.sourceType == ActionSourceType::Player)
						{
							auto& type = state.gameInfo->actionTypes->at(actionType.id);
							for (auto& effect : type.OnComplete)
							{
								effect->execute(state, *this, state.players[j].continuousAction[i].targets);
							}
						}
					}

					//Delete the ContinuousAction
					state.players[j].continuousAction.erase(state.players[j].continuousAction.begin() + i);
					i--;
					//Stop executing this action
					continue;
				}
			}

		}
	}
		
	void ForwardModel::endTick(GameState& state) const
	{
		state.currentTick++;

		executeOnTriggerEffects(state);
		checkEntitiesContinuousActionIsComplete(state);
		checkPlayerContinuousActionIsComplete(state);
	}
	

	void ForwardModel::spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const
	{
		auto entityID = state.addEntity(entityType, playerID, position);

		std::vector<ActionTarget> targets = { ActionTarget::createEntityActionTarget(entityID) };
		for(const auto& onSpawnEffect : onEntitySpawnEffects)
		{
			if (onSpawnEffect.validTargets.find(entityType.id) == onSpawnEffect.validTargets.end())
				continue;

			auto isValid = true;
			for (const auto& condition : onSpawnEffect.conditions)
			{
				if (!condition->isFulfilled(state, targets))
				{
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				for (const auto& effect : onSpawnEffect.effects)
				{
					effect->execute(state, *this, targets);
				}
			}
		}
	}
}
