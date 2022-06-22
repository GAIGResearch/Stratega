#include <Stratega/ForwardModel/ActionSpace.h>
#pragma warning(disable: 5045)
namespace SGA
{
	std::vector<Action> ActionSpace::generateActions(const GameState& gameState, int playerID) const
	{
		std::vector<Action> bucket;
		//Generate entities actions
		for (const auto& sourceEntity : gameState.getEntities())
		{
			if (sourceEntity.getOwnerID() != playerID)
				continue;

			for (const auto& actionInfo : sourceEntity.getAttachedActions())
			{
				const auto& actionType = gameState.getGameInfo()->getActionType(actionInfo.actionTypeID);

				bool generateContinuousAction = true;
				//Check if action is continuos
				if (actionType.isContinuous())
				{
					//Check if entity is already executing it
					for (const auto& action : sourceEntity.getContinuousActions())
					{
						if (action.getActionTypeID() == actionType.getID())
						{
							//This entity cant execute the action
							generateContinuousAction = false;

							//Give the posibility to abort it
							bucket.emplace_back(Action::createAbortEntityAction(playerID, sourceEntity.getID(), action.getContinuousActionID()));
						}
					}
				}

				if (!generateContinuousAction)
					continue;

				// Check if this action can be executed		
				if (gameState.getCurrentTick() - actionInfo.lastExecutedTick < actionType.getCooldown())
					continue;
				if (!gameState.canExecuteAction(sourceEntity, actionType))
					continue;

				// Generate all actions
				if (actionType.getTargets().size() == 0/*TargetType::None*/)
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
		for (const auto& actionInfo : player.getAttachedActions())
		{
			const auto& actionType = gameState.getGameInfo()->getActionType(actionInfo.actionTypeID);
			bool generateContinuousAction = true;
			//Check if action is continuos
			if (actionType.isContinuous())
			{
				//Check if entity is already executing it
				for (auto& action : player.getContinuousActions())
				{
					if (action.getActionTypeID() == actionType.getID())
					{
						//This entity cant execute the action
						generateContinuousAction = false;

						//Give the posibility to abort it
						bucket.emplace_back(Action::createAbortPlayerAction(player.getID(), action.getContinuousActionID()));
					}
				}
			}

			if (!generateContinuousAction)
				continue;

			// Check if this action can be executed

			if (gameState.getCurrentTick() - actionInfo.lastExecutedTick < actionType.getCooldown())
				continue;
			if (!gameState.canExecuteAction(player, actionType))
				continue;

			// Generate all actions
			if (actionType.getTargets().size() == 0/*TargetType::None*/)
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
		if (gameState.getGameType() == GameType::TBS)
		{
			bucket.emplace_back(Action::createEndAction(playerID));
		}

		return bucket;
	}

	std::vector<Action> ActionSpace::generateQueueActions(const GameState& gameState, int playerID) const
	{
		std::vector<Action> bucket;
		
		if (gameState.getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceType==ActionQueuePack::ActionSourceType::Player)
		{
			//Generate player actions
			auto& player = *gameState.getPlayer(playerID);
			auto& actionType = gameState.getGameInfo()->getActionType(gameState.getActionQueuesConst().getActionFromPlayerQueue(player.getID()).actionTypeID);
			
			//Generate EndTurnAction
			if (actionType.isEndTick())
			{
				bucket.emplace_back(Action::createEndAction(playerID, &actionType));
				return bucket;
			}

			//Check if action is continuos
			if (actionType.isContinuous())
			{
				//Check if entity is already executing it
				for (auto& action : player.getContinuousActions())
				{
					if (action.getActionTypeID() == actionType.getID())
					{
						//Give the posibility to abort it
						bucket.emplace_back(Action::createAbortPlayerAction(player.getID(), action.getContinuousActionID()));
					}
				}
			}
			// Check if this action can be executed

			if (!gameState.canExecuteAction(player, actionType))
				return bucket;

			// Generate all actions
			if (actionType.getTargets().size() == 0/*TargetType::None*/)
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
		else
		{
			int sourceEntityID = gameState.getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceID;
			if (gameState.getEntityConst(sourceEntityID))
			{
				auto& sourceEntity = *gameState.getEntityConst(sourceEntityID);
				const auto& actionType = gameState.getGameInfo()->getActionType(gameState.getActionQueuesConst().getActionFromPlayerQueue(playerID).actionTypeID);

				bool generateContinuousAction = true;
				//Check if action is continuos
				if (actionType.isContinuous())
				{
					//Check if entity is already executing it
					for (const auto& action : sourceEntity.getContinuousActions())
					{
						if (action.getActionTypeID() == actionType.getID())
						{
							//This entity cant execute the action
							generateContinuousAction = false;

							//Give the posibility to abort it
							bucket.emplace_back(Action::createAbortEntityAction(playerID, sourceEntity.getID(), action.getContinuousActionID()));
						}
					}
				}

				if (generateContinuousAction)
				{
					// Generate all actions
					if (actionType.getTargets().size() == 0/*TargetType::None*/)
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
		}		

		return bucket;
	}

	// <summary>
	// Implemented for Unit-MCTS, creat action space for each unit.
	// </summary>
	std::vector<Action> ActionSpace::generateUnitActions(const GameState& gameState, Entity& e, int playerID, bool generateEnd) const
	{
		std::vector<Action> bucket;

		//std::cout << "generating Unit Action" << std::endl;
		auto& sourceEntity = e;

		//Generate entities actions
		for (const auto& actionInfo : sourceEntity.getAttachedActions())
		{
			auto& actionType = gameState.getGameInfo()->getActionType(actionInfo.actionTypeID);

			bool generateContinuousAction = true;
			//Check if action is continuos
			if (actionType.isContinuous())
			{
				//Check if entity is already executing it
				for (const auto& action : sourceEntity.getContinuousActions())
				{
					if (action.getActionTypeID() == actionType.getID())
					{
						//This entity cant execute the action
						generateContinuousAction = false;

						//Give the posibility to abort it

						bucket.emplace_back(Action::createAbortEntityAction(playerID, sourceEntity.getID(), action.getContinuousActionID()));
					}
				}
			}
			if (!generateContinuousAction)
				continue;

			// Check if this action can be executed		
			if (gameState.getCurrentTick() - actionInfo.lastExecutedTick < actionType.getCooldown())
				continue;
			if (!gameState.canExecuteAction(sourceEntity, actionType))
				continue;

			// Generate all actions
			if (actionType.getTargets().size() == 0/*TargetType::None*/)
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

		//Generate EndTurnAction
		if (gameState.getGameType() == GameType::TBS && generateEnd)
		{
			//std::cout << "!!!! generating End action" << std::endl;
			bucket.emplace_back(Action::createEndAction(playerID));
		}

		return bucket;
	}

	std::vector<std::vector<ActionTarget>> ActionSpace::productActionTargets(const std::vector<std::vector<ActionTarget>>& lists) const
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
			action.setOwnerID(sourceEntity.getOwnerID());
			action.getTargets().emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.getID()));

			for (auto& target : targetsProduct)
			{
				action.getTargets().emplace_back(target);
			}

			if (actionType.isContinuous())
				action.setActionFlag(ActionFlag::ContinuousAction);

			bool isValidAction = true;
			for (auto& actionTargetType : actionType.getTargets())
				for (const auto& condition : actionTargetType.second)
				{
					if (!condition->isFullfiled(state, action.getTargets()))
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
			action.setOwnerID(sourcePlayer.getID());
			action.getTargets().emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.getID()));

			for (auto& target : targetsProduct)
			{
				action.getTargets().emplace_back(target);
			}

			if (actionType.isContinuous())
				action.setActionFlag(ActionFlag::ContinuousAction);

			bool isValidAction = true;
			for (auto& actionTargetType : actionType.getTargets())
				for (const auto& condition : actionTargetType.second)
				{
					if (!condition->isFullfiled(state, action.getTargets()))
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

		for (auto& type : action.getTargets())
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.getType())
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, entity.getPosition(), type.first.getSamplingMethod());
				break;
			case TargetType::Tile: newTargets = generateTileTargets(state, entity.getPosition(), type.first.getTileTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, entity.getPosition(), type.first.getGroupEntityTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.getGroupEntityTypes());
				break;
			case TargetType::Object: newTargets = generateObjectTargets(state, entity, type.first.getGroupEntityTypes());
				break;
			case TargetType::SlotObject: newTargets = generateSlotObjectTargets(state, entity, type.first.getGroupEntityTypes());
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.getTechnologyTypes());
				break;
			case TargetType::ContinuousAction: newTargets = generateContinuousActionTargets(state, entity);
				break;
			case TargetType::None: return {};
			}

			targets.emplace_back(newTargets);
		}

		return targets;
	}

	std::vector<std::vector<ActionTarget>> ActionSpace::generateTargets(const GameState& state, const Entity& entity, const std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>>& sourceTargets) const
	{
		std::vector<std::vector<ActionTarget>> targets;

		for (auto& type : sourceTargets)
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.getType())
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, entity.getPosition(), type.first.getSamplingMethod());
				break;
			case TargetType::Tile: newTargets = generateTileTargets(state, entity.getPosition(), type.first.getTileTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, entity.getPosition(), type.first.getGroupEntityTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.getGroupEntityTypes());
				break;
			case TargetType::Object: newTargets = generateObjectTargets(state, entity, type.first.getGroupEntityTypes());
				break;
			case TargetType::SlotObject: newTargets = generateSlotObjectTargets(state, entity, type.first.getGroupEntityTypes());
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.getTechnologyTypes());
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

		for (auto& type : action.getTargets())
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.getType())
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, type.first.getSamplingMethod());
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, type.first.getGroupEntityTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::Tile: newTargets = generateTileTargets(state,type.first.getTileTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.getGroupEntityTypes());
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.getTechnologyTypes());
				break;
			case TargetType::None: return {};
				break;
			case TargetType::SlotObject: return {};
				break;
			case TargetType::Object: return {};
				break;
			case TargetType::ContinuousAction: return {};
				break;
			}

			targets.emplace_back(newTargets);
		}

		return targets;
	}
	
	std::vector<std::vector<ActionTarget>> ActionSpace::generateTargets(const GameState& state, const Player& /*player*/, const std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>>& sourceTargets) const
	{
		std::vector<std::vector<ActionTarget>> targets;

		for (auto& type : sourceTargets)
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.getType())
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, type.first.getSamplingMethod());
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, type.first.getGroupEntityTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::Tile: newTargets = generateTileTargets(state,type.first.getTileTypes(), type.first.getSamplingMethod());
				break;
			case TargetType::EntityType: newTargets = generateEntityTypeTargets(state, type.first.getGroupEntityTypes());
				break;
			case TargetType::Technology: newTargets = generateTechnologyTargets(state, type.first.getTechnologyTypes());
				break;
			case TargetType::None: return {};
				break;
			case TargetType::SlotObject: return {};
				break;
			case TargetType::Object: return {};
				break;
			case TargetType::ContinuousAction: return {};
				break;
			}

			targets.emplace_back(newTargets);
		}

		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generatePositionTargets(const GameState& gameState, const Vector2f& position, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto positions = samplingMethod->getPositions(gameState, position);

		for (auto& element : positions)
		{
			targets.emplace_back(ActionTarget::createPositionActionTarget(Vector2f(element.x, element.y)));
		}

		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateTileTargets(const GameState& gameState, const Vector2f& position, const std::unordered_set<TileTypeID>& tileTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto positions = samplingMethod->getPositions(gameState, position);

		for (auto& element : positions)
		{
			if (tileTypeIDs.find(gameState.getTileAtConst(element).getTileTypeID()) != tileTypeIDs.end())
				targets.emplace_back(ActionTarget::createTileActionTarget(Vector2f(element.x, element.y)));
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

	std::vector<ActionTarget> ActionSpace::generateTileTargets(const GameState& gameState, const std::unordered_set<TileTypeID>& tileTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const
	{
		std::vector<ActionTarget> targets;

		auto positions = samplingMethod->getPositions(gameState);

		for (auto& element : positions)
		{
			if (tileTypeIDs.find(gameState.getTileAtConst(element).getTileTypeID()) != tileTypeIDs.end())
				targets.emplace_back(ActionTarget::createTileActionTarget(Vector2f(element.x, element.y)));
		}
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateEntityTypeTargets(const GameState& /*gameState*/, const std::unordered_set<EntityTypeID>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;
		for (const auto& entityTypeID : entityTypeIDs)
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

	std::vector<ActionTarget> ActionSpace::generateGroupTargets(const GameState& gameState, const Vector2f& position, const std::unordered_set<EntityTypeID>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const
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

	std::vector<ActionTarget> ActionSpace::generateObjectTargets(const GameState& /*gameState*/,  const Entity& source, const std::unordered_set<EntityTypeID>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;

		for (const auto& object : source.getInventory())
		{
			if (entityTypeIDs.find(object.getEntityTypeID()) != entityTypeIDs.end())
				targets.emplace_back(ActionTarget::createObjectActionTarget(object.getID()));
		}
		
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateSlotObjectTargets(const GameState& /*gameState*/,  const Entity& source, const std::unordered_set<EntityTypeID>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;

		for (const auto& object : source.getSlots())
		{
			if (entityTypeIDs.find(object.first.getEntityTypeID()) != entityTypeIDs.end())
				targets.emplace_back(ActionTarget::createSlotObjectActionTarget(object.first.getID()));
		}
		
		return targets;
	}

	std::vector<ActionTarget> ActionSpace::generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs) const
	{
		std::vector<ActionTarget> targets;

		for (const auto& technoloTreeType : gameState.getGameInfo()->getTechnologyTreeCollection().technologyTreeTypes)
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
			targets.emplace_back(ActionTarget::createContinuousActionActionTarget(action.getContinuousActionID()));
		}
		return targets;
	}

	Action ActionSpace::generateSelfAction(const Entity& sourceEntity, const ActionType& actionType) const
	{
		Action selfAction(&actionType);
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.getID()));
		selfAction.setActionTargets(actionTargets);
		return selfAction;
	}

	Action ActionSpace::generateSelfAction(const Player& sourcePlayer, const ActionType& actionType) const
	{
		Action selfAction(&actionType);
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.getID()));
		selfAction.setActionTargets(actionTargets);
		return selfAction;
	}

}