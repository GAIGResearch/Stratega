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
				if (loseCondition->isFullfiled(state, targets))
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
				if (!winCondition->isFullfiled(state, targets))
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
			const auto& continuousActions = sourcePlayer.getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				if (continuousActions[i].continuousActionID == continuousActionID)
				{
					auto& actionType = continuousActions[i].getActionType();

					//Execute OnAbort Effects				
					for (auto& effect : actionType.getOnAbort())
					{
						effect->execute(state, *this, continuousActions[i].targets);
					}

					//Remove continuous action
					sourcePlayer.removeContinuousAction(i);
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
					for (auto& effect : actionType.getOnAbort())
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
		if (actionType.getSourceType() == ActionSourceType::Entity)
		{
			auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
			for (auto& effect : type.getOnStart())
			{
				effect->execute(state, *this, newAction.targets);
			}

			auto& executingEntity = *newAction.targets[0].getEntity(state);
			executingEntity.getContinuousActions().emplace_back(newAction);
		}
		else if (actionType.getSourceType() == ActionSourceType::Player)
		{
			auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
			for (auto& effect : type.getOnStart())
			{
				effect->execute(state, *this, newAction.targets);
			}

			auto& executingPlayer = newAction.targets[0].getPlayer(state);
			executingPlayer.addContinuousAction(newAction);
		}
	}

	void ForwardModel::executeNormalAction(GameState& state, const Action& action) const
	{
		const auto& actionType = action.getActionType();
		if (actionType.getSourceType() == ActionSourceType::Entity)
		{
			auto& executingEntity = *action.targets[0].getEntity(state);
			executingEntity.setActionTicks(action.getActionTypeID(), state.getCurrentTick());
		}
		else
		{
			// Remember when the action was executed
			auto& executingPlayer = action.targets[0].getPlayer(state);
			// ToDo We should probably find a way to avoid this loop
			for (size_t i = 0; i < executingPlayer.getAttachedActions().size(); i++)
			{
				auto& actionInfo = executingPlayer.getAttachedAction(i);
				if (actionInfo.actionTypeID == action.getActionTypeID())
				{
					executingPlayer.setActionLastTick(i, state.getCurrentTick());
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
			for (const auto& entity : state.getEntities())
			{
				if (onTickEffect.validTargets.find(entity.getEntityTypeID()) == onTickEffect.validTargets.end())
					continue;
				std::vector<ActionTarget> targets;
				targets.emplace_back(ActionTarget::createEntityActionTarget(entity.getID()));
				auto isValid = true;
				for (const auto& condition : onTickEffect.conditions)
				{
					if (!condition->isFullfiled(state, targets))
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
		auto& entities = state.getEntities();
		for(auto et = entities.begin(); et != entities.end(); et++)
		{
			auto& continuousActions = et->getContinuousActions();
			auto it = continuousActions.begin();
			while(it != continuousActions.end())
			{
				auto& actionType = it->getActionType();
				//Add one elapsed tick
				it->elapsedTicks++;
				//Execute OnTick Effects
				if (actionType.getSourceType() == ActionSourceType::Entity)
				{
					auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
					for (auto& effect : type.getOnTick())
					{
						effect->execute(state, *this, it->targets);
					}
				}
				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.getTriggerComplete())
				{
					if (!condition->isFullfiled(state, it->targets))
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
						if (actionType.getSourceType() == ActionSourceType::Entity)
						{
							auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
							for (auto& effect : type.getOnComplete())
							{
								effect->execute(state, *this, it->targets);
							}
						}
					}

					//Delete the ContinuousAction
					it = continuousActions.erase(it);
				}
				else it++;
			}
		}
	}

	void ForwardModel::checkPlayerContinuousActionIsComplete(GameState& state) const
	{
		//Player continuous Action
		std::vector<Player> players = state.getPlayers();
		for (size_t j = 0; j < players.size(); j++)
		{
			const auto& continuousActions = players[j].getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				auto& actionType = continuousActions[i].getActionType();
				//Add one elapsed tick
				players[j].advanceContinuousAction(i);

				//Execute OnTick Effects
				if (actionType.getSourceType() == ActionSourceType::Player)
				{
					auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
					for (auto& effect : type.getOnTick())
					{
						effect->execute(state, *this, continuousActions[i].targets);
					}
				}

				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.getTriggerComplete())
				{
					if (!condition->isFullfiled(state, continuousActions[i].targets))
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
						if (actionType.getSourceType() == ActionSourceType::Player)
						{
							auto& type = state.getGameInfo()->actionTypes->at(actionType.getID());
							for (auto& effect : type.getOnComplete())
							{
								effect->execute(state, *this, continuousActions[i].targets);
							}
						}
					}

					//Delete the ContinuousAction
					players[j].removeContinuousAction(i);
					i--;
					//Stop executing this action
					continue;
				}
			}

		}
	}
		
	void ForwardModel::endTick(GameState& state) const
	{
		state.incTick();

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
				if (!condition->isFullfiled(state, targets))
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

	void ForwardModel::addWinConditions(std::vector<std::shared_ptr<Condition>>& conditions)
	{
		winConditions.emplace_back(conditions);
	}

	void ForwardModel::addLoseConditions(std::vector<std::shared_ptr<Condition>>& conditions)
	{
		loseConditions.emplace_back(conditions);
	}

	void ForwardModel::addOnTickEffect(OnTickEffect& ote) 
	{ 
		onTickEffects.emplace_back(ote); 
	}

	void ForwardModel::addOnEntitySpawnEffect(OnEntitySpawnEffect& ose) 
	{ 
		onEntitySpawnEffects.emplace_back(ose); 
	}
}
