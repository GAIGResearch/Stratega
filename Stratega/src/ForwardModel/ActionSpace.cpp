#include <Stratega/ForwardModel/ActionSpace.h>

namespace SGA
{
	std::vector<Action> ActionSpace::generateActions(const GameState& gameState, int playerID) const
	{
		std::vector<Action> bucket;
		//Generate entities actions
		for (const auto& sourceEntity : gameState.getEntities())
		{
			if (sourceEntity.ownerID != playerID)
				continue;
			
			for (const auto& actionInfo : sourceEntity.getAttachedActions())
			{
				auto& actionType = gameState.getGameInfo()->getActionType(actionInfo.actionTypeID);
				
				bool generateContinuousAction = true;
				//Check if action is continuos
				if (actionType.isContinuous)
				{
					//Check if entity is already executing it
					for (const auto& action : sourceEntity.getContinuousActions())
					{
						if (action.getActionTypeID() == actionType.id)
						{
							//This entity cant execute the action
							generateContinuousAction = false;

							//Give the posibility to abort it
							bucket.emplace_back(Action::createAbortAction(playerID, sourceEntity.id, action.continuousActionID));
						}
					}
				}

				if (!generateContinuousAction)
					continue;
				
				// Check if this action can be executed		
				if (gameState.getCurrentTick() - actionInfo.lastExecutedTick < actionType.cooldownTicks)
					continue;
				if (!gameState.canExecuteAction(sourceEntity, actionType))
					continue;

				// Generate all actions
				if(actionType.actionTargets.size() == 0/*TargetType::None*/)
				{
					// Self-actions do not have a target, only a source
					bucket.emplace_back(generateSelfAction(sourceEntity, actionType));
				}
				else
				{
					auto targets = generateTargets(gameState, sourceEntity, actionType);
					generateActions(gameState, sourceEntity, actionType, targets, bucket);
				}
			}
		}

		//Generate player actions
		auto& player = *gameState.getPlayer(playerID);
		for (const auto& actionInfo : player.attachedActions)
		{
			auto& actionType = gameState.getGameInfo()->getActionType(actionInfo.actionTypeID);
			bool generateContinuousAction = true;
			//Check if action is continuos
			if (actionType.isContinuous)
			{
				//Check if entity is already executing it
				for (auto& action : player.continuousAction)
				{
					if (action.getActionTypeID() == actionType.id)
					{
						//This entity cant execute the action
						generateContinuousAction = false;

						//Give the posibility to abort it
						bucket.emplace_back(Action::createAbortAction(player.id, action.continuousActionID));
					}
				}
			}

			if (!generateContinuousAction)
				continue;
			
			// Check if this action can be executed
			
			if (gameState.getCurrentTick() - actionInfo.lastExecutedTick < actionType.cooldownTicks)
				continue;
			if (!gameState.canExecuteAction(player, actionType))
				continue;

			// Generate all actions
			if (actionType.actionTargets.size() == 0/*TargetType::None*/)
			{
				// Self-actions do not have a target, only a source
				bucket.emplace_back(generateSelfAction(player, actionType));
			}
			else
			{
				auto targets = generateTargets(gameState, player, actionType);
				generateActions(gameState, player, actionType, targets, bucket);
			}
		}		
		
		//Generate EndTurnAction
		if(gameState.getGameType() == GameType::TBS)
		{
			bucket.emplace_back(Action::createEndAction(playerID));
		}
		
		return bucket;
	}

	auto productActionTargets(const std::vector<std::vector<ActionTarget>>& lists)
	{		
		std::vector<std::vector<ActionTarget>> result;

		if (lists.size() == 0)
			return result;
		
		if (std::find_if(std::begin(lists), std::end(lists),
			[](auto e) -> bool { return e.empty(); }) != std::end(lists)) {
			return result;
		}
		for (const auto& e : lists[0])
		{
			result.push_back({ e });
		}
		for (size_t i = 1; i < lists.size(); ++i) {
			std::vector<std::vector<ActionTarget>> temp;
			for (auto& e : result)
			{
				for (const auto& f : lists[i])
				{
					auto e_tmp = e;
					e_tmp.push_back(f);
					temp.push_back(e_tmp);
				}
			}
			result = temp;
		}
		return result;
	}
	
	void ActionSpace::generateActions(const GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const
	{		
		for (auto& targetsProduct : productActionTargets(targets))
		{
			Action action(&actionType);
			action.ownerID = sourceEntity.ownerID;
			action.targets.emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.id));
			
			for (auto& target : targetsProduct)
			{
				action.targets.emplace_back(target);
			}

			if (actionType.isContinuous)
				action.actionTypeFlags = ActionFlag::ContinuousAction;

			bool isValidAction = true;
			for (auto& actionTargetType : actionType.actionTargets)
			for (const auto& condition : actionTargetType.second)
			{
				if (!condition->isFulfilled(state, action.targets))
				{
					isValidAction = false;
					break;
				}
			}

			if (isValidAction)
			{
				actionBucket.emplace_back(action);
			}
		}
	}

	void ActionSpace::generateActions(const GameState& state, const Player& sourcePlayer, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const
	{
		for (auto& targetsProduct : productActionTargets(targets))
		{
			Action action(&actionType);
			action.ownerID = sourcePlayer.id;
			action.targets.emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.id));

			for (auto& target : targetsProduct)
			{
				action.targets.emplace_back(target);
			}

			if (actionType.isContinuous)
				action.actionTypeFlags = ActionFlag::ContinuousAction;

			bool isValidAction = true;
			for (auto& actionTargetType : actionType.actionTargets)
				for (const auto& condition : actionTargetType.second)
				{
					if (!condition->isFulfilled(state, action.targets))
					{
						isValidAction = false;
						break;
					}
				}

			if (isValidAction)
			{
				actionBucket.emplace_back(action);
			}
		}
	}
	
	std::vector<std::vector<ActionTarget>> ActionSpace::generateTargets(const GameState& state, const Entity& entity, const ActionType& action) const
	{
		std::vector<std::vector<ActionTarget>> targets;
		
		for (auto& type : action.actionTargets)
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.type)
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, entity.position, type.first.samplingMethod);
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, entity.position, type.first.groupEntityTypes, type.first.samplingMethod);
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.groupEntityTypes);
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.technologyTypes);
				break;
			case TargetType::ContinuousAction: newTargets = generateContinuousActionTargets(state, entity);
				break;
			case TargetType::None: return {};
			}

			targets.emplace_back(newTargets);
		}
		
		return targets;
	}

	std::vector<std::vector<ActionTarget>> ActionSpace::generateTargets(const GameState& state, const Player& /*entity*/, const ActionType& action) const
	{
		std::vector<std::vector<ActionTarget>> targets;

		for (auto& type : action.actionTargets)
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.type)
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, type.first.samplingMethod);
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, type.first.groupEntityTypes, type.first.samplingMethod);
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.groupEntityTypes);
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.technologyTypes);
				break;
			case TargetType::None: return {};
			}

			targets.emplace_back(newTargets);
		}

		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generatePositionTargets(const GameState& gameState, const Vector2f& position, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto positions=samplingMethod->getPositions(gameState, position);

		for (auto& element : positions)
		{
			targets.emplace_back(ActionTarget::createPositionActionTarget(Vector2f(element.x, element.y)));
		}
		
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generatePositionTargets(const GameState& gameState, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto positions = samplingMethod->getPositions(gameState);

		for (auto& element : positions)
		{
			targets.emplace_back(ActionTarget::createPositionActionTarget(Vector2f(element.x, element.y)));
		}
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateEntityTypeTargets(const GameState& /*gameState*/, const std::unordered_set<EntityTypeID>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;
		for(const auto& entityTypeID : entityTypeIDs)
		{
			targets.push_back(ActionTarget::createEntityTypeActionTarget(entityTypeID));
		}

		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateGroupTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto entitiesIDs = samplingMethod->getEntities(gameState, entityTypeIDs);

		for (const auto& entityID : entitiesIDs)
		{			
			if (entityTypeIDs.find(gameState.getEntityConst(entityID)->getEntityTypeID()) != entityTypeIDs.end())
			{
				targets.emplace_back(ActionTarget::createEntityActionTarget(entityID));
			}
		}
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateGroupTargets(const GameState& gameState,  const Vector2f& position, const std::unordered_set<EntityTypeID>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto entitiesIDs = samplingMethod->getEntities(gameState, position, entityTypeIDs);
		
		for (const auto& entityID : entitiesIDs)
		{
			if (entityTypeIDs.find(gameState.getEntityConst(entityID)->getEntityTypeID()) != entityTypeIDs.end())
			{
				targets.emplace_back(ActionTarget::createEntityActionTarget(entityID));
			}
		}
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;
		
		for (const auto& technoloTreeType : gameState.getGameInfo()->technologyTreeCollection->technologyTreeTypes)
		{
			for (auto& technology : technoloTreeType.second.technologies)
			{
				if (entityTypeIDs.find(technology.second.id) != entityTypeIDs.end())
				{
					targets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(technology.second.id));
				}
			}			
		}
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateContinuousActionTargets(const GameState& /*gameState*/, const Entity& sourceEntity) const
	{
		std::vector<ActionTarget> targets;

		for (const auto& action : sourceEntity.getContinuousActions())
		{
			targets.emplace_back(ActionTarget::createContinuousActionActionTarget(action.continuousActionID));
		}
		return targets;
	}
	
	Action ActionSpace::generateSelfAction(const Entity& sourceEntity, const ActionType& actionType) const
	{
		Action selfAction(&actionType);
		selfAction.targets = { ActionTarget::createEntityActionTarget(sourceEntity.id) };
		return selfAction;
	}

	Action ActionSpace::generateSelfAction(const Player& sourcePlayer, const ActionType& actionType) const
	{
		Action selfAction(&actionType);
		selfAction.targets = { ActionTarget::createPlayerActionTarget(sourcePlayer.id) };
		return selfAction;
	}

}