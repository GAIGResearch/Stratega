#include <Stratega/ForwardModel/ForwardModel.h>
#pragma warning(disable: 5045)
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

	std::vector <Action> ForwardModel::generateUnitActions(const GameState& state, Entity e, int playerID, bool generateEnd) const
	{
		return actionSpace->generateUnitActions(state, e, playerID, generateEnd);
	}

	std::vector<Action> ForwardModel::generatePlayerActions(const GameState& state, int playerID, bool generateEnd) const {
		return actionSpace->generatePlayerActions(state, playerID, generateEnd);
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

        // each lose condition defined several rules. all rules are fullfied will activate this lose condition.
        // any lose condition fullfiled will terminate the game
		for (auto& loseConditionType : loseConditions)
		{
            
            bool allFullfiled = true;
			for (auto& loseCondition : loseConditionType)
			{
				if (!loseCondition->isFullfiled(state, targets))
                    allFullfiled = false;
					//return true;
			}
            if(allFullfiled)
                return allFullfiled;
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
		if (action.getTargets()[0].getType() == ActionTarget::PlayerReference)
		{
			auto& sourcePlayer = action.getTargets()[0].getPlayer(state);
			int continuousActionID = action.getTargets()[1].getContinuousActionID();

			//Search continuousAction to abort
			const auto& continuousActions = sourcePlayer.getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				if (continuousActions[i].getContinuousActionID() == continuousActionID)
				{
					auto& actionType = continuousActions[i].getActionType();

					//Execute OnAbort Effects				
					for (auto& effect : actionType.getOnAbort())
					{
						effect->execute(state, *this, continuousActions[i].getTargets());
					}

					//Remove continuous action
					sourcePlayer.removeContinuousAction(static_cast<int>(i));
					i--;
				}
			}
		}
		else
		{
			auto& sourceEntity = *action.getTargets()[0].getEntity(state);
			int continuousActionID = action.getTargets()[1].getContinuousActionID();

			//Search continuousAction to abort
			std::vector<Action>& continuousActions = sourceEntity.getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				if (continuousActions[i].getContinuousActionID() == continuousActionID)
				{
					auto& actionType = continuousActions[i].getActionType();

					//Execute OnAbort Effects				
					for (auto& effect : actionType.getOnAbort())
					{
						effect->execute(state, *this, continuousActions[i].getTargets());
					}

					//Remove continuous action
					continuousActions.erase(continuousActions.begin() + static_cast<int>(i));
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
		newAction.setContinuousActionID(state.getNextContinuousActionID());
		state.incNextContinuousActionID();
		newAction.getTargets().emplace_back(ActionTarget::createContinuousActionActionTarget(newAction.getContinuousActionID()));

		//If is continues we execute OnStart Effects
		//and we add the action to the list of continuous actions
		if (actionType.getSourceType() == ActionSourceType::Entity)
		{
			const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
			for (const auto& effect : type.getOnStart())
			{
				effect->execute(state, *this, newAction.getTargets());
			}

			auto& executingEntity = *newAction.getTargets()[0].getEntity(state);
			executingEntity.getContinuousActions().emplace_back(newAction);
		}
		else if (actionType.getSourceType() == ActionSourceType::Player)
		{
			const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
			for (const auto& effect : type.getOnStart())
			{
				effect->execute(state, *this, newAction.getTargets());
			}

			auto& executingPlayer = newAction.getTargets()[0].getPlayer(state);
			executingPlayer.addContinuousAction(newAction);
		}
	}

	void ForwardModel::executeNormalAction(GameState& state, const Action& action) const
	{
		const auto& actionType = action.getActionType();
		if (actionType.getSourceType() == ActionSourceType::Entity)
		{
			auto& executingEntity = *action.getTargets()[0].getEntity(state);
			executingEntity.setActionTicks(action.getActionTypeID(), state.getCurrentTick());
		}
		else
		{
			// Remember when the action was executed
			auto& executingPlayer = action.getTargets()[0].getPlayer(state);
			// ToDo We should probably find a way to avoid this loop
			for (size_t i = 0; i < executingPlayer.getAttachedActions().size(); i++)
			{
				auto& actionInfo = executingPlayer.getAttachedAction(static_cast<int>(i));
				if (actionInfo.actionTypeID == action.getActionTypeID())
				{
					executingPlayer.setActionLastTick(static_cast<int>(i), state.getCurrentTick());
					break;
				}
			}
		}

		action.execute(state, *this);
	}

	void ForwardModel::executeAction(GameState& state, const Action& action) const
	{
		switch (action.getActionFlag())
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
			if (onTickEffect.type == SourceOnTickEffectType::Entity)
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
			else if (onTickEffect.type == SourceOnTickEffectType::Player)
			{
				for (const auto& player : state.getPlayers())
				{
					std::vector<ActionTarget> targets;
					targets.emplace_back(ActionTarget::createPlayerActionTarget(player.getID()));
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
			else
			{
				//GameState source

				std::vector<ActionTarget> targets;
				targets.emplace_back(ActionTarget::createGameStateActionTarget());
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

	void ForwardModel::executeOnAdvanceEffects(GameState& state) const
	{
		// Execute OnTick-trigger
		for (const auto& onAdvanceEffect : onAdvanceEffects)
		{
			if (onAdvanceEffect.type == SourceOnTickEffectType::Entity)
			{
				for (const auto& entity : state.getEntities())
				{
					if (onAdvanceEffect.validTargets.find(entity.getEntityTypeID()) == onAdvanceEffect.validTargets.end())
						continue;
					std::vector<ActionTarget> targets;
					targets.emplace_back(ActionTarget::createEntityActionTarget(entity.getID()));
					auto isValid = true;
					for (const auto& condition : onAdvanceEffect.conditions)
					{
						if (!condition->isFullfiled(state, targets))
						{
							isValid = false;
							break;
						}
					}

					if (isValid)
					{
						for (const auto& effect : onAdvanceEffect.effects)
						{
							effect->execute(state, *this, targets);
						}
					}
				}
			}
			else if (onAdvanceEffect.type == SourceOnTickEffectType::Player)
			{
				for (const auto& player : state.getPlayers())
				{
					std::vector<ActionTarget> targets;
					targets.emplace_back(ActionTarget::createPlayerActionTarget(player.getID()));
					auto isValid = true;
					for (const auto& condition : onAdvanceEffect.conditions)
					{
						if (!condition->isFullfiled(state, targets))
						{
							isValid = false;
							break;
						}
					}

					if (isValid)
					{
						for (const auto& effect : onAdvanceEffect.effects)
						{
							effect->execute(state, *this, targets);
						}
					}
				}
			}
			else
			{
				//GameState source

				std::vector<ActionTarget> targets;
				targets.emplace_back(ActionTarget::createGameStateActionTarget());
				auto isValid = true;
				for (const auto& condition : onAdvanceEffect.conditions)
				{
					if (!condition->isFullfiled(state, targets))
					{
						isValid = false;
						break;
					}
				}

				if (isValid)
				{
					for (const auto& effect : onAdvanceEffect.effects)
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
		for (auto et = entities.begin(); et != entities.end(); et++)
		{
			auto& continuousActions = et->getContinuousActions();
			auto it = continuousActions.begin();
			while (it != continuousActions.end())
			{
				auto& actionType = it->getActionType();
				//Add one elapsed tick
				it->incTicks();
				//Execute OnTick Effects
				if (actionType.getSourceType() == ActionSourceType::Entity)
				{
					const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
					for (const auto& effect : type.getOnTick())
					{
						effect->execute(state, *this, it->getTargets());
					}
				}
				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.getTriggerComplete())
				{
					if (!condition->isFullfiled(state, it->getTargets()))
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
							const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
							for (const auto& effect : type.getOnComplete())
							{
								effect->execute(state, *this, it->getTargets());
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
		std::vector<Player>& players = state.getPlayers();
		for (size_t j = 0; j < players.size(); j++)
		{
			const auto& continuousActions = players[j].getContinuousActions();
			for (size_t i = 0; i < continuousActions.size(); i++)
			{
				auto& actionType = continuousActions[i].getActionType();
				//Add one elapsed tick
				players[j].advanceContinuousAction(static_cast<int>(i));

				//Execute OnTick Effects
				if (actionType.getSourceType() == ActionSourceType::Player)
				{
					const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
					for (const auto& effect : type.getOnTick())
					{
						effect->execute(state, *this, continuousActions[i].getTargets());
					}
				}

				//Check if action is complete
				bool isComplete = true;
				for (const auto& condition : actionType.getTriggerComplete())
				{
					if (!condition->isFullfiled(state, continuousActions[i].getTargets()))
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
							const auto& type = state.getGameInfo()->getActionTypes().at(actionType.getID());
							for (const auto& effect : type.getOnComplete())
							{
								effect->execute(state, *this, continuousActions[i].getTargets());
							}
						}
					}

					//Delete the ContinuousAction
					players[j].removeContinuousAction(static_cast<int>(i));
					i--;
					//Stop executing this action
					continue;
				}
			}

		}
	}

	void ForwardModel::removeExpiredBuffs(GameState& state) const
	{
		//Remove expired Buffs
		auto& entities = state.getEntities();
		for (auto et = entities.begin(); et != entities.end(); et++)
		{
			auto& buffs = et->getBuffs();
			auto it = buffs.begin();
			while (it != buffs.end())
			{
				it->incrementElapseTicks();
				if (it->getElapsedTicks() >= it->getDurationTicks())
				{
					it = buffs.erase(it);
					et->recomputeStats();
				}
				else it++;
			}
		}

		auto& players = state.getPlayers();
		for (auto player = players.begin(); player != players.end(); player++)
		{
			auto& buffs = player->getBuffs();
			auto it = buffs.begin();
			while (it != buffs.end())
			{
				it->incrementElapseTicks();
				if (it->getElapsedTicks() >= it->getDurationTicks())
				{
					it = buffs.erase(it);
					player->recomputeStats(state);
				}
				else it++;
			}
		}
	}

	void ForwardModel::executeOnTickEntityActions(GameState& state) const
	{
		//ExecuteOnTickEntityActions
		std::vector<Action> bucket;
		auto& entities = state.getEntities();

		//Generate entities actions
		for (const auto& sourceEntity : entities)
		{
			for (const auto& actionTypeID : sourceEntity.getEntityType().getOnTickActionIDs())
			{
				const auto& actionType = state.getGameInfo()->getActionType(actionTypeID);

				if (!state.canExecuteAction(sourceEntity, actionType))
					continue;

				// Generate all actions
				if (actionType.getTargets().size() == 0/*TargetType::None*/)
				{
					// Self-actions do not have a target, only a source
					bucket.emplace_back(actionSpace->generateSelfAction(sourceEntity, actionType));
				}
				else
				{
					auto targets = actionSpace->generateTargets(state, sourceEntity, actionType);
					actionSpace->generateActions(state, sourceEntity, actionType, targets, bucket);
				}
			}
		}

		//Execute actions
		for (auto& action : bucket)
		{
			executeAction(state, action);
		}
	}

	void ForwardModel::endTick(GameState& state) const
	{
		state.incTick();
		removeExpiredBuffs(state);
		executeOnTriggerEffects(state);
		executeOnTickEntityActions(state);
		checkEntitiesContinuousActionIsComplete(state);
		checkPlayerContinuousActionIsComplete(state);
	}

	void ForwardModel::spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const
	{
		auto entityID = state.addEntity(entityType, playerID, position);

		std::vector<ActionTarget> targets = { ActionTarget::createEntityActionTarget(entityID) };
		for (const auto& onSpawnEffect : onEntitySpawnEffects)
		{
			if (onSpawnEffect.validTargets.find(entityType.getID()) == onSpawnEffect.validTargets.end())
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

	void ForwardModel::addOnAdvanceEffect(OnTickEffect& ote)
	{
		onAdvanceEffects.emplace_back(ote);
	}

	void ForwardModel::addOnEntitySpawnEffect(OnEntitySpawnEffect& ose)
	{
		onEntitySpawnEffects.emplace_back(ose);
	}

	void ForwardModel::modifyEntityParameterByIndex(Entity& entity, int parameterIndex, double newValue) const
	{
		//Get parameter
		auto& parameterValue = entity.getRawParameterAt(parameterIndex);
		//Modify it
		parameterValue = newValue;
		//Keep it in bounds min/max
		const double min = entity.getMinParameterAt(parameterIndex);
		const double max = entity.getMaxParameterAt(parameterIndex);
		parameterValue = std::max(min, std::min(parameterValue, max));
	}

	void ForwardModel::modifyStateParameterByIndex(GameState& state, int parameterIndex, double newValue) const
	{
		//Get parameter
		auto& parameterValue = state.getRawParameterAt(parameterIndex);
		//Modify it
		parameterValue = newValue;
		//Keep it in bounds min/max
		const double min = state.getMinParameterAt(parameterIndex);
		const double max = state.getMaxParameterAt(parameterIndex);
		parameterValue = std::max(min, std::min(parameterValue, max));
	}
	void ForwardModel::modifyPlayerParameterByIndex(Player& player, int parameterIndex, double newValue) const
	{
		//Get parameter
		auto& parameterValue = player.getRawParameterAt(parameterIndex);
		//Modify it
		parameterValue = newValue;
		//Keep it in bounds min/max
		const double min = player.getMinParameterAt(parameterIndex);
		const double max = player.getMaxParameterAt(parameterIndex);
		parameterValue = std::max(min, std::min(parameterValue, max));
	}
}