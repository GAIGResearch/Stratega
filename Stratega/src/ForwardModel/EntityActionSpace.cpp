#include <ForwardModel/EntityActionSpace.h>

namespace SGA
{
	std::vector<Action> EntityActionSpace::generateActions(GameState& gameState, int playerID)
	{
		std::vector<Action> bucket;
		//Generate entities actions
		for (auto& sourceEntity : gameState.entities)
		{
			if (sourceEntity.ownerID != playerID)
				continue;

			
			for (const auto& actionInfo : sourceEntity.attachedActions)
			{
				auto& actionType = gameState.getActionType(actionInfo.actionTypeID);
				
				bool generateContinuousAction = true;
				//Check if action is continuos
				if (actionType.isContinuous)
				{
					//Check if entity is already executing it
					for (auto& action : sourceEntity.continuousAction)
					{
						if (action.actionTypeID == actionType.id)
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
				if (gameState.currentTick - actionInfo.lastExecutedTick < actionType.cooldownTicks)
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


		////Generate player actions
		//auto& player = *gameState.getPlayer(playerID);
		//for (const auto& actionInfo : player.attachedActions)
		//{
		//	auto& actionType = gameState.getActionType(actionInfo.actionTypeID);
		//	bool generateContinuousAction = true;
		//	//Check if action is continuos
		//	if (actionType.isContinuous)
		//	{
		//		//Check if entity is already executing it
		//		for (auto& action : player.continuousAction)
		//		{
		//			if (action.actionTypeID == actionType.id)
		//			{
		//				//This entity cant execute the action
		//				generateContinuousAction = false;

		//				//Give the posibility to abort it
		//				bucket.emplace_back(Action::createAbortAction(player.id, action.continuousActionID));
		//			}
		//		}
		//	}

		//	if (!generateContinuousAction)
		//		continue;
		//	
		//	// Check if this action can be executed
		//	
		//	if (gameState.currentTick - actionInfo.lastExecutedTick < actionType.cooldownTicks)
		//		continue;
		//	if (!gameState.canExecuteAction(player, actionType))
		//		continue;

		//	
		//	//// Generate all actions
		//	//if (actionType.actionTargets == TargetType::None)
		//	//{
		//	//	// Self-actions do not have a target, only a source
		//	//	bucket.emplace_back(generateSelfAction(player, actionType));
		//	//}
		//	//else
		//	//{
		//		/*auto targets = generateTargets(gameState, player, actionType);
		//		generateActions(gameState, player, actionType, targets, bucket);*/
		//	//}
		//}
		
		
		//Generate EndTurnAction
		bucket.emplace_back(Action::createEndAction(playerID));
		return bucket;
	}

	auto product(const std::vector<std::vector<ActionTarget>>& lists)
	{
		
		std::vector<std::vector<ActionTarget>> result;

		if (lists.size() == 0)
			return result;
		
		if (std::find_if(std::begin(lists), std::end(lists),
			[](auto e) -> bool { return e.size() == 0; }) != std::end(lists)) {
			return result;
		}
		for (auto& e : lists[0]) {
			result.push_back({ e });
		}
		for (size_t i = 1; i < lists.size(); ++i) {
			std::vector<std::vector<ActionTarget>> temp;
			for (auto& e : result) {
				for (auto f : lists[i]) {
					auto e_tmp = e;
					e_tmp.push_back(f);
					temp.push_back(e_tmp);
				}
			}
			result = temp;
		}
		return result;
	}
	
	void EntityActionSpace::generateActions( GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket)
	{
		
		for (auto& targetsProduct : product(targets))
		{
			Action action;
			action.actionTypeID = actionType.id;
			action.ownerID = sourceEntity.ownerID;
			action.targets.emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.id));
			
			for (auto& target : targetsProduct)
			{
				action.targets.emplace_back(target);
			}

			if (actionType.isContinuous)
				action.actionTypeFlags = ContinuousAction;



			bool isValidAction = true;
			for (auto& actionTargetType : actionType.actionTargets)
			for (const auto& condition : actionTargetType.second)
			{
				if (!condition->isFullfilled(state, action.targets))
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
		
		/*Action action;
		action.actionTypeID = actionType.id;
		action.ownerID = sourceEntity.ownerID;
		
		action.targets.emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.id));
		action.targets.emplace_back(ActionTarget::createEntityActionTarget(sourceEntity.id));

		if (actionType.isContinuous)
			action.actionTypeFlags = ContinuousAction;
		
		std::vector<Action> allActions;
		for (const auto& target : targets)
		{
			action.targets[1] = target;
			bool isValidAction = true;
			for (const auto& condition : actionType.targetConditions)
			{
				if (!condition->isFullfilled(state, action.targets))
				{
					isValidAction = false;
					break;
				}
			}

			if (isValidAction)
			{
				actionBucket.emplace_back(action);
			}
		}*/
	}

	void EntityActionSpace::generateActions(GameState& state, const Player& sourcePlayer, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket)
	{

		for (auto& targetsProduct : product(targets))
		{
			Action action;
			action.actionTypeID = actionType.id;
			action.ownerID = sourcePlayer.id;
			action.targets.emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.id));

			for (auto& target : targetsProduct)
			{
				action.targets.emplace_back(target);
			}

			if (actionType.isContinuous)
				action.actionTypeFlags = ContinuousAction;



			bool isValidAction = true;
			for (auto& actionTargetType : actionType.actionTargets)
				for (const auto& condition : actionTargetType.second)
				{
					if (!condition->isFullfilled(state, action.targets))
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
		
		/*Action action;
		action.actionTypeID = actionType.id;
		action.ownerID = sourcePlayer.id;

		action.targets.emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.id));
		action.targets.emplace_back(ActionTarget::createPlayerActionTarget(sourcePlayer.id));

		if (actionType.isContinuous)
			action.actionTypeFlags = ContinuousAction;

		std::vector<Action> allActions;
		for (const auto& target : targets)
		{
			action.targets[1] = target;
			bool isValidAction = true;
			for (const auto& condition : actionType.targetConditions)
			{
				if (!condition->isFullfilled(state, action.targets))
				{
					isValidAction = false;
					break;
				}
			}

			if (isValidAction)
			{
				actionBucket.emplace_back(action);
			}

		}*/
	}
	
	std::vector<std::vector<ActionTarget>> EntityActionSpace::generateTargets(const GameState& state, const Entity& entity, const ActionType& action)
	{
		std::vector<std::vector<ActionTarget>> targets;

		
		for (auto& type : action.actionTargets)
		{
			std::vector<ActionTarget> newTargets;
			switch (type.first.type)
			{
			case TargetType::Position: newTargets = generatePositionTargets(state, entity.position, type.first.shapeType, type.first.shapeSize);
				break;
			case TargetType::Entity: newTargets = generateGroupTargets(state, type.first.groupEntityTypes);
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
		throw std::runtime_error("Tried generating action-targets for unknown target-type");
	}

	//std::vector<ActionTarget> EntityActionSpace::generateTargets(const GameState& state, const Player& entity, const ActionType& action)
	//{
	//	//switch (action.actionTargets.type)
	//	//{
	//	//case TargetType::Position: return generatePositionTargets(state);
	//	//case TargetType::Entity: return generateGroupTargets(state, action.actionTargets.groupEntityTypes);
	//	//case TargetType::Technology: return generateTechnologyTargets(state, action.actionTargets.technologyTypes);
	//	////case TargetType::ContinuousAction: return generateContinuousActionTargets(state, entity);
	//	//case TargetType::None: return {};
	//	//}
	//	return {};
	//	throw std::runtime_error("Tried generating action-targets for unknown target-type");
	//}

	std::vector<ActionTarget> EntityActionSpace::generatePositionTargets(const GameState& gameState, const Vector2f& position, ShapeType shape, int shapeSize)
	{
		auto isValidPos = [&](float x, float y)
		{
			if (gameState.board.get(x, y).tileTypeID == -1)
				return false;
			
			switch (shape)
			{
				case ShapeType::Square: return true;
				case ShapeType::Circle: return Vector2f{ x, y }.distance(position) <= shapeSize;
				default: return false;
			}
		};

		// Iterate over an rectangle as large as 'shapeSize' and take every valid position
		auto startCheckPositionX = std::max<int>(0, position.x - shapeSize);
		auto endCheckPositionX = std::min<int>(gameState.board.getWidth() - 1, position.x + shapeSize);
		auto startCheckPositionY = std::max<int>(0, position.y - shapeSize);
		auto endCheckPositionY = std::min<int>(gameState.board.getHeight() - 1, position.y + shapeSize);
		std::vector<ActionTarget> targets;
		for (auto x = startCheckPositionX; x <= endCheckPositionX; x++)
		{
			for (auto y = startCheckPositionY; y <= endCheckPositionY; y++)
			{
				if(isValidPos(x, y))
					targets.emplace_back(ActionTarget::createPositionActionTarget(Vector2f(x, y)));
			}
		}
		return targets;
	}

	std::vector<ActionTarget> EntityActionSpace::generatePositionTargets(const GameState& gameState)
	{
		//TODO ONLY WHAT CAN SEE?
		auto isValidPos = [&](float x, float y)
		{
			if (gameState.board.get(x, y).tileTypeID == -1)
				return false;
			else
				return true;
		};
		
		std::vector<ActionTarget> targets;		
		for (auto x = 0; x <= gameState.board.getWidth()-1; x++)
		{
			for (auto y = 0; y <= gameState.board.getHeight()-1; y++)
			{
				if (isValidPos(x, y))
				targets.emplace_back(ActionTarget::createPositionActionTarget(Vector2f(x, y)));
			}
		}
		return targets;
	}

	std::vector<ActionTarget> EntityActionSpace::generateEntityTypeTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs)
	{
		std::vector<ActionTarget> targets;
		for(const auto& entityTypeID : entityTypeIDs)
		{
			targets.push_back(ActionTarget::createEntityTypeActionTarget(entityTypeID));
		}

		return targets;
	}

	std::vector<ActionTarget> EntityActionSpace::generateGroupTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs)
	{
		std::vector<ActionTarget> targets;
		for (const auto& entity : gameState.entities)
		{
			if(entityTypeIDs.find(entity.typeID) != entityTypeIDs.end())
			{
				targets.emplace_back(ActionTarget::createEntityActionTarget(entity.id));
			}
		}
		return targets;
	}

	std::vector<ActionTarget> EntityActionSpace::generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs)
	{
		std::vector<ActionTarget> targets;
		
		for (const auto& technoloTreeType : gameState.technologyTreeCollection->technologyTreeTypes)
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

	std::vector<ActionTarget> EntityActionSpace::generateContinuousActionTargets(const GameState& gameState, const Entity& sourceEntity)
	{
		std::vector<ActionTarget> targets;

		for (auto& action : sourceEntity.continuousAction)
		{
			targets.emplace_back(ActionTarget::createContinuousActionActionTarget(action.continuousActionID));
		}
		return targets;
	}
	
	Action EntityActionSpace::generateSelfAction(const Entity& sourceEntity, const ActionType& actionType)
	{
		Action selfAction;
		selfAction.actionTypeID = actionType.id;
		selfAction.targets = { ActionTarget::createEntityActionTarget(sourceEntity.id) };
		return selfAction;
	}

	Action EntityActionSpace::generateSelfAction(const Player& sourcePlayer, const ActionType& actionType)
	{
		Action selfAction;
		selfAction.actionTypeID = actionType.id;
		selfAction.targets = { ActionTarget::createPlayerActionTarget(sourcePlayer.id) };
		return selfAction;
	}

}