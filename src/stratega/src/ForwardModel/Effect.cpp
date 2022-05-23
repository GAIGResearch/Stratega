#include <Stratega/ForwardModel/Effect.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <boost/random.hpp>

#include "Stratega/ForwardModel/FunctionParameter.h"
#pragma warning(disable: 5045)
namespace SGA
{
	void EffectPack::execute(SGA::GameState& state, const SGA::ForwardModel& fm, const std::vector<SGA::ActionTarget>& targetsVector)
	{
		std::vector<std::vector<ActionTarget>> generatedTargets;

		if(targetsVector[0].getType()==ActionTarget::EntityReference)
			generatedTargets = fm.getActionSpace()->generateTargets(state, *targetsVector[0].getEntity(state), actionTargets);
		else if(targetsVector[0].getType()==ActionTarget::PlayerReference)
			generatedTargets = fm.getActionSpace()->generateTargets(state, targetsVector[0].getPlayer(state), actionTargets);


		std::vector<std::vector<ActionTarget>> actionTargetLists;
		for (auto& target : targetsVector)
		{
			std::vector<ActionTarget> actionTargetTemp;
			actionTargetTemp.emplace_back(target);
			actionTargetLists.emplace_back(actionTargetTemp);
		}
		for (auto& target : generatedTargets)
		{
			actionTargetLists.emplace_back(target);
		}
		
		
		auto actionTargetsResult=fm.getActionSpace()->productActionTargets(actionTargetLists);

	
		
		for (auto& targets : actionTargetsResult)
		{
			bool isValidAction = true;
			//check each sigle target list if the condition is true
			for (auto& target : actionTargets)
			{
				for (const auto& condition : target.second)
				{
					if (!condition->isFullfiled(state, targets))
					{
						isValidAction = false;
						break;
					}
				}
			}
				
			if (isValidAction)
			{
				if (effectPackType == EffectPackType::Sample)
				{
					for (const auto& effect : effects)
					{
						if (effect.which() == 0)
							boost::get<std::shared_ptr<Effect>>(effect)->execute(state, fm, targets);
						else
						{
							boost::get<std::shared_ptr<EffectPack>>(effect)->execute(state, fm, targets);
						}
					}
				}
				else if (effectPackType == EffectPackType::Conditional)
				{
					//Execute conditions
					//Execute effects

					bool isConditionFullfilled = true;
					for (const auto& conditionPair : conditionEffects.first)
					{
						if (!conditionPair->isFullfiled(state, targets))
						{
							isConditionFullfilled = false;
							break;
						}
					}

					if(isConditionFullfilled)
						for (const auto& effect : conditionEffects.second)
						{
							if (effect.which() == 0)
								boost::get<std::shared_ptr<Effect>>(effect)->execute(state, fm, targets);
							else
							{
								boost::get<std::shared_ptr<EffectPack>>(effect)->execute(state, fm, targets);
							}
						}
					
				}
				else if (effectPackType == EffectPackType::Random)
				{
					//Execute conditions
					//Execute effects
					auto rndEngine = state.getRndEngine();
					//Get a float random from 0->1
					boost::random::uniform_real_distribution<float> distribution(0, 1);
					float rndNumber = distribution(rndEngine);
					//std::cout << "Random number: " << rndNumber << std::endl;

					for (const auto& randomPair : randomEffects)
					{
						if (randomPair.first <= rndNumber)
						{
							for (const auto& effect : randomPair.second)
							{
								if (effect.which() == 0)
									boost::get<std::shared_ptr<Effect>>(effect)->execute(state, fm, targets);
								else
								{
									boost::get<std::shared_ptr<EffectPack>>(effect)->execute(state, fm, targets);
								}
							}
						}
					}
				}
			}				
		}		
	}

	ModifyResource::ModifyResource(const std::string exp, const std::vector<FunctionParameter>& parameters) :		
		Effect(exp),
		resourceReference(parameters.at(0)),
		amountParameter(parameters.at(1))
	{

	}
	
	void ModifyResource::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getRawParameterValue(state, targets);
		auto amount = amountParameter.getConstant(state, targets);

		targetResource += amount;
		auto& param = resourceReference.getParameter(state, targets);
		int parameterIndex = param.getIndex();
		if (resourceReference.isEntityParameter(targets))
		{
			auto& entity = resourceReference.getEntity(state, targets);
			fm.modifyEntityParameterByIndex(entity, parameterIndex, targetResource);
		}
		else if (resourceReference.isPlayerParameter(targets))
		{
			auto& player = resourceReference.getPlayer(state, targets);
			fm.modifyPlayerParameterByIndex(player, parameterIndex, targetResource);
		}
		else if (resourceReference.isTileParameter(targets))
		{
			auto& tile = resourceReference.getTile(state, targets);
			fm.modifyTileParameterByIndex(tile, parameterIndex, targetResource);
		}
		else
		{
			fm.modifyStateParameterByIndex(state, parameterIndex, targetResource);
		}
		
	}
	
	EnqueueAction::EnqueueAction(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		source(parameters.at(0)),
		actionType(parameters.at(1))
	{

	}
	
	void EnqueueAction::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{
		auto action = actionType.getActionType(state, targets);
		if (source.isPlayerReference(targets))
		{
			auto player = source.getPlayer(state, targets);
			state.getActionQueues().addActionToPlayerQueue(ActionQueuePack::ActionSourceType::Player, player.getID(),player.getID(), action.getID());
		}
		else if (source.isEntityReference(targets))
		{
			auto entity = source.getEntity(state, targets);
			state.getActionQueues().addActionToPlayerQueue(ActionQueuePack::ActionSourceType::Entity, entity.getOwnerID(), entity.getID(), action.getID());
		}		
	}

	ApplyBuff::ApplyBuff(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		buffReference(parameters.at(1)),
		buffTicks(parameters.at(2)),
        entityParam(parameters.at(0))
		
	{

	}
	
	void ApplyBuff::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		auto& buffType = buffReference.getBuffType(state, targets);
		auto ticks = buffTicks.getConstant(state, targets);

		if(entityParam.getType()==FunctionParameter::Type::EntityPlayerReference)
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto* player = state.getPlayer(entity.getOwnerID());
			auto newBuff = Buff::createBuff(
			   buffType, player->getID(), static_cast<int>(ticks));
			player->addBuff(std::move(newBuff));
			player->recomputeStats(state);
		}
		else
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto newBuff = Buff::createBuff(
			   buffType, entity.getID(), static_cast<int>(ticks));
			entity.addBuff(std::move(newBuff));
			entity.recomputeStats();
		}
	}

	RemoveBuff::RemoveBuff(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		buffReference(parameters.at(1)),
        entityParam(parameters.at(0))		
	{

	}
	
	void RemoveBuff::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		auto& buffType = buffReference.getBuffType(state, targets);

		if (entityParam.getType() == FunctionParameter::Type::EntityPlayerReference)
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto* player = state.getPlayer(entity.getOwnerID());
			player->removeBuffsOfType(buffType);
			player->recomputeStats(state);
		}
		else
		{
			auto& entity = entityParam.getEntity(state, targets);
			entity.removeBuffsOfType(buffType);
			entity.recomputeStats();
		}
	}

	RemoveAllBuffs::RemoveAllBuffs(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters.at(0))
	{

	}
	
	void RemoveAllBuffs::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		if (entityParam.getType() == FunctionParameter::Type::EntityPlayerReference)
		{
			auto& entity = entityParam.getEntity(state, targets);
			auto* player = state.getPlayer(entity.getOwnerID());
			player->removeAllBuffs();
			player->recomputeStats(state);
		}
		else
		{
			auto& entity = entityParam.getEntity(state, targets);
			entity.removeAllBuffs();
			entity.recomputeStats();
		}
	}

	ChangeResource::ChangeResource(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		resourceReference(parameters.at(0)),
		amountParameter(parameters.at(1))
	{

	}

	void ChangeResource::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		double amount = amountParameter.getConstant(state, targets);
		int parameterIndex = resourceReference.getParameter(state, targets).getIndex();
		
		if (resourceReference.isEntityParameter(targets))
		{
			auto& entitySource = resourceReference.getEntity(state, targets);
			fm.modifyEntityParameterByIndex(entitySource, parameterIndex, amount);
		}
		else if (resourceReference.isPlayerParameter(targets))
		{
			auto& playerSource = resourceReference.getPlayer(state, targets);
			fm.modifyPlayerParameterByIndex(playerSource, parameterIndex, amount);
		}
		else if (resourceReference.isTileParameter(targets))
		{
			auto& tile = resourceReference.getTile(state, targets);
			fm.modifyTileParameterByIndex(tile, parameterIndex, amount);
		}
		else
		{
			fm.modifyStateParameterByIndex(state, parameterIndex, amount);
		}
		
	}
	
	Attack::Attack(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp), 
		resourceReference(parameters.at(0)),
		amountParameter(parameters.at(1))
	{

	}
	
	void Attack::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = resourceReference.getEntity(state, targets);
		auto targetResource = resourceReference.getRawParameterValue(state, targets);
		int parameterIndex = resourceReference.getParameter(state, targets).getIndex();
		auto amount = amountParameter.getConstant(state, targets);
		
		//Remove to the parameter with buffs appliead the amount
        targetResource -= amount;
		//std::cout << "Attacked " << targetResource << std::endl;
		fm.modifyEntityParameterByIndex(entity, parameterIndex, targetResource);

		if(targetResource <= 0)
			entity.flagRemove();

		
	}
	
	AttackWithArmorUnderCover::AttackWithArmorUnderCover(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp), 
		sourceReference(parameters.at(0)),
		armorReference(parameters.at(1)),
		healthReference(parameters.at(2)),
		amountParameter(parameters.at(3)),
		penaltyParameter(parameters.at(4))
	{

	}
	
	void AttackWithArmorUnderCover::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		auto& target = armorReference.getEntity(state, targets);
		auto targetResource = armorReference.getRawParameterValue(state, targets);
		int parameterIndex = armorReference.getParameter(state, targets).getIndex();
		auto amount = amountParameter.getConstant(state, targets);
		auto penalty = penaltyParameter.getConstant(state, targets);
		
		//Compute if enemy is cover
		//Checks if target entity has a wall direction of source
		auto& entity = sourceReference.getEntity(state, targets);

		auto pushDir = entity.getPosition() - target.getPosition();
		pushDir = pushDir.normalized();
		auto wallPositionCheck = target.getPosition() + pushDir;

		const auto& tile = state.getTileAt(static_cast<int>(wallPositionCheck.x), static_cast<int>(wallPositionCheck.y));

		if (!tile.isWalkable())
		{
			//It has a cover in front
			amount -= amount * (penalty / 100);
		}

		//Remove armor
        targetResource -= amount;
		double amountRemaining = 0;
		fm.modifyEntityParameterByIndex(target, parameterIndex, targetResource);
		if (targetResource < 0)
		{
			amountRemaining = -targetResource;
			targetResource = 0;			

			//Lets remove the remaining to health
			targetResource = healthReference.getRawParameterValue(state, targets);
			parameterIndex = healthReference.getParameter(state, targets).getIndex();

			targetResource -= amountRemaining;

			fm.modifyEntityParameterByIndex(target, parameterIndex, targetResource);

			if (targetResource <= 0)
				target.flagRemove();
		}
	}
	AttackAroundWithArmor::AttackAroundWithArmor(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp), 
		sourceReference(parameters.at(0)),
		targetPositionReference(parameters.at(1)),
		armorReference(parameters.at(2)),
		healthReference(parameters.at(3)),
		amountParameter(parameters.at(4)),
		samplingMethod(std::move(std::make_unique<Neighbours>()))
	{
		samplingMethod->shapeType = Neighbours::ShapeType::Cross;
		samplingMethod->shapeSize = 1;
	}
	
	void AttackAroundWithArmor::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		//auto& armorParame = armorReference.getEntity(state, targets);
		auto targetResource = armorReference.getRawParameterValue(state, targets);
		//int parameterIndex = armorReference.getParameter(state, targets).getIndex();
		auto amount = amountParameter.getConstant(state, targets);
		
		auto targetPosition = targetPositionReference.getPosition(state, targets);

		//Get entities around position		
		auto entities = samplingMethod->getEntities(state, targetPosition, {});

		for (auto& entID : entities)
		{
			auto* target = state.getEntity(entID);

			if (!target)
				continue;

			//Remove armor
			targetResource = target->getParameter(armorReference.getParameter(state, targets).getName());

			targetResource -= amount;
			double amountRemaining = 0;
			
			fm.modifyEntityByParameterByName(*target, armorReference.getParameter(state, targets).getName(), targetResource);

			if (targetResource < 0)
			{
				amountRemaining = -targetResource;
				targetResource = 0;

				//Lets remove the remaining to health
				targetResource = target->getParameter(healthReference.getParameter(state, targets).getName());
				targetResource -= amountRemaining;

				fm.modifyEntityByParameterByName(*target, healthReference.getParameter(state, targets).getName(), targetResource);

				if (targetResource <= 0)
					target->flagRemove();
			}
		}		
	}

	Empty::Empty(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0])
	{
	}
	
	void Empty::execute(GameState& /*state*/, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& /*targets*/) const
	{
	}

	Push::Push(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}
	
	void Push::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		auto pushDir = target.getPosition() - entity.getPosition();
		auto newTargetPos = target.getPosition() + pushDir;
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }) && state.isOccupied(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
	}
	
	PushAndHit::PushAndHit(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]),
		targetParam(parameters[1]),
		resourceReference(parameters.at(2)),
		amountParameter(parameters.at(3))
	{

	}
	
	void PushAndHit::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		auto pushDir = target.getPosition() - entity.getPosition();
		pushDir = pushDir.normalized();
		auto newTargetPos = target.getPosition() + pushDir;
		
		auto* hittedEntity = state.getEntityAt(newTargetPos);
		if (state.isInBounds(newTargetPos) && state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}

		//Deals damage
		auto targetResource = resourceReference.getRawParameterValue(state, targets);
		int parameterIndex = resourceReference.getParameter(state, targets).getIndex();
		auto amount = amountParameter.getConstant(state, targets);

		//Remove to the parameter with buffs appliead the amount
		targetResource -= amount;

		fm.modifyEntityParameterByIndex(target, parameterIndex, targetResource);

		if (targetResource <= 0)
			target.flagRemove();

		if (hittedEntity)
		{
			std::cout << "There is another entity";
			//Deal damage to hitted entity
			auto param = hittedEntity->getParameter(resourceReference.getParameter(state, targets).getName());

			fm.modifyEntityByParameterByName(*hittedEntity, resourceReference.getParameter(state, targets).getName(), param - amount);

			if (param - amount <= 0)
				hittedEntity->flagRemove();

			targetResource -= amount;

			//And damage the target
			fm.modifyEntityParameterByIndex(target, parameterIndex, targetResource);

			if (targetResource <= 0)
				target.flagRemove();
		}

		
	}

	PushAroundPositionAndHit::PushAroundPositionAndHit(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]),
		targetParam(parameters[1]),
		resourceReference(parameters.at(2)),
		amountParameter(parameters.at(3)),
		samplingMethod(std::move(std::make_unique<Neighbours>()))
	{
		samplingMethod->shapeType = Neighbours::ShapeType::Cross;
		samplingMethod->shapeSize = 1;
	}
	
	void PushAroundPositionAndHit::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		//auto& entity = entityParam.getEntity(state, targets);
		auto targetPosition = targetParam.getPosition(state, targets);

		//Get entities around position		
		auto entities= samplingMethod->getEntities(state, targetPosition, {});

		auto amount = amountParameter.getConstant(state, targets);

		//Deal damage to entity in target position
		auto* targetPositionEntity = state.getEntityAt(targetPosition);
		std::string parameterName = resourceReference.getParameter(state, targets).getName();

		if (targetPositionEntity)
		{
			//Deals damage
			//auto currValue = targetPositionEntity->getParameter(parameterName);
			
			//Deal damage to hitted entity
			auto param = targetPositionEntity->getParameter(resourceReference.getParameter(state, targets).getName());

			fm.modifyEntityByParameterByName(*targetPositionEntity, resourceReference.getParameter(state, targets).getName(), param - amount);

			if (param - amount <= 0)
				targetPositionEntity->flagRemove();
		}

		for (auto& entID : entities)
		{
			auto* pushedEntity = state.getEntity(entID);

			
			
			if (!pushedEntity||pushedEntity->getPosition()==targetPosition)
				continue;

			auto actionTypes = pushedEntity->getActionTypes(*state.getGameInfo());
			auto it = find_if(actionTypes.begin(), actionTypes.end(), [&actionTypes](const ActionType& obj) {return obj.getName() == "Move"; });
			
			//Hardcoded way to check if entity can move
			//Once we had to actions calling other actions we will remove this
			if (it == actionTypes.end())
				continue;

			auto pushDir = pushedEntity->getPosition() - targetPosition;
			pushDir = pushDir.normalized();
			auto newTargetPos = pushedEntity->getPosition() + pushDir;

			auto* hittedEntity = state.getEntityAt(newTargetPos);
			if (state.isInBounds(newTargetPos) && state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
			{
				pushedEntity->setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
			}

			if (hittedEntity)
			{
				std::cout << "There is another entity";
				//Deal damage to hitted entity
				auto param = hittedEntity->getParameter(resourceReference.getParameter(state, targets).getName());

				fm.modifyEntityByParameterByName(*hittedEntity, resourceReference.getParameter(state, targets).getName(), param - amount);

				if (param - amount <= 0)
					hittedEntity->flagRemove();

				param = pushedEntity->getParameter(resourceReference.getParameter(state, targets).getName());

				//And damage the target
				fm.modifyEntityByParameterByName(*pushedEntity, resourceReference.getParameter(state, targets).getName(), param - amount);

				if (param - amount <= 0)
					pushedEntity->flagRemove();
			}
		}
	}
	
	PickUp::PickUp(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}

	void PickUp::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& entity = entityParam.getEntity(state, targets);
		auto& object = targetParam.getEntity(state, targets);

		fm.executeOnAddedObjectInventory(state, entity, object);

		entity.addObject(object);

		object.flagRemove();
	}
	
	DropObject::DropObject(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), objectParam(parameters[1]), positionParam(parameters[2])
	{

	}

	void DropObject::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{
		auto& entity = entityParam.getEntity(state, targets);
		Entity object = objectParam.getObject(state, targets);
		auto position = positionParam.getPosition(state, targets);

		entity.removeObject(object.getID());
		state.addEntity(object, -1, position);
	}
	
	RemoveObject::RemoveObject(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), objectParam(parameters[1])
	{

	}

	void RemoveObject::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{
		auto& entity = entityParam.getEntity(state, targets);
		Entity object = objectParam.getObject(state, targets);

		entity.removeObject(object.getID());
	}

	AttackProbability::AttackProbability(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp), 
		resourceReference(parameters.at(0)),
		amountParameter(parameters.at(1)),
		probabilityParameter(parameters.at(2))
	{

	}
	
	void AttackProbability::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = resourceReference.getEntity(state, targets);
		auto targetResource = resourceReference.getRawParameterValue(state, targets);
		int parameterIndex = resourceReference.getParameter(state, targets).getIndex();
		auto amount = amountParameter.getConstant(state, targets);
		auto probability = probabilityParameter.getConstant(state, targets);
		
		boost::random::uniform_int_distribution<unsigned int> distribution(0, 100);
       
		//Get chance to attack
		if(distribution(state.getRndEngine()) < probability)
		{
			targetResource -= amount;
			fm.modifyEntityParameterByIndex(entity, parameterIndex, targetResource);
			auto targetvalueResource = resourceReference.getParameterValue(state, targets);
			if (targetvalueResource <= 0)
				entity.flagRemove();
		}
	}

	Move::Move(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: 
		Effect(exp), 
		entityParam(parameters[0]), targetPositionParam(parameters[1])
	{
	}
	
	void Move::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& entity = entityParam.getEntity(state, targets);
		auto targetPosition = targetPositionParam.getPosition(state, targets);
		
		if (fm.getGameType() == GameType::TBS)
		{
			entity.setPosition( { std::floor(targetPosition.x), std::floor(targetPosition.y) } );
		}
		else if(fm.getGameType() == GameType::RTS )
		{
			const auto* rtsFM = dynamic_cast<const RTSForwardModel*>(&fm);

			//Get end position of current path
			Vector2f oldTargetPos;
			oldTargetPos.x = static_cast<double>(entity.getPath().m_straightPath[(entity.getPath().m_nstraightPath - 1) * 3]);
			oldTargetPos.y = static_cast<double>(entity.getPath().m_straightPath[((entity.getPath().m_nstraightPath - 1) * 3) + 2]);

			// Compute a new path if the entity doesn't have one or the new target is different from the old one
			if (entity.getPath().isEmpty() || oldTargetPos.distance(targetPosition) > 0.0001)
			{
				Path path = rtsFM->findPath(state, entity.getPosition(), targetPosition);
				entity.setPath(path);
				entity.incPathIndex();
			}
		}
	}

	SetToMaximum::SetToMaximum(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		targetResource(parameters[0])
	{
	}

	void SetToMaximum::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		const auto& param = targetResource.getParameter(state, targets);
		auto& paramValue = targetResource.getRawParameterValue(state, targets);

		paramValue = param.getMaxValue();
	}

	TransferEffect::TransferEffect(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		sourceParam(parameters[0]), targetParam(parameters[1]), amountParam(parameters[2])
	{
	}

	void TransferEffect::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& sourceType = sourceParam.getParameter(state, targets);
		auto sourceValue = sourceParam.getRawParameterValue(state, targets);
		auto targetValue = targetParam.getRawParameterValue(state, targets);
		auto amount = amountParam.getConstant(state, targets);

		// Compute how much the source can transfer, if the source does not have enough just take everything
		amount = std::min(amount, sourceValue - sourceType.getMinValue());
		// Transfer
		sourceValue -= amount;
		targetValue = targetValue + amount;

		int parameterSourceIndex = sourceParam.getParameter(state, targets).getIndex();
		int parameterTargetIndex = targetParam.getParameter(state, targets).getIndex();

		if (!sourceParam.isPlayerParameter(targets))
		{
			auto& entitySource = sourceParam.getEntity(state, targets);			
			fm.modifyEntityParameterByIndex(entitySource, parameterSourceIndex, sourceValue);
		}
		else
		{
			auto& playerSource = sourceParam.getPlayer(state, targets);
			fm.modifyPlayerParameterByIndex(playerSource, parameterSourceIndex, sourceValue);
		}
		
		if (!targetParam.isPlayerParameter(targets))
		{
			auto& entityTarget = targetParam.getEntity(state, targets);
			fm.modifyEntityParameterByIndex(entityTarget, parameterTargetIndex, targetValue);
		}
		else
		{
			auto& playerTarget = targetParam.getPlayer(state, targets);
			fm.modifyPlayerParameterByIndex(playerTarget, parameterTargetIndex, targetValue);
		}
	}

	ChangeOwnerEffect::ChangeOwnerEffect(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp),
		targetEntityParam(parameters[0]), 
		playerParam(parameters[1])
		
	{
	}
	
	void ChangeOwnerEffect::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		auto& targetEntity = targetEntityParam.getEntity(state, targets);
		auto& newOwner = playerParam.getPlayer(state, targets);
		targetEntity.setOwnerID(newOwner.getID());
	}

	RemoveEntityEffect::RemoveEntityEffect(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), targetEntityParam(parameters[0])
	{
	}

	void RemoveEntityEffect::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		auto& targetEntity = targetEntityParam.getEntity(state, targets);
		targetEntity.flagRemove();
	}

	ResearchTechnology::ResearchTechnology(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), playerParam(parameters[0]),
		  technologyTypeParam(parameters[1])
	{
	}

	void ResearchTechnology::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		state.researchTechnology(targetPlayer.getID(), targets[1].getTechnologyID());
	}

	SpawnEntityRandom::SpawnEntityRandom(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		sourceEntityParam(parameters[0]), 
		targetEntityTypeParam(parameters[1])
	{
	}

	void SpawnEntityRandom::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if (fm.getGameType()==GameType::TBS)
		{
			auto& sourceEntity = sourceEntityParam.getEntity(state, targets);
			const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);

			for(int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					Vector2i spawnPos{ static_cast<int>(sourceEntity.x()) + dx, static_cast<int>(sourceEntity.y()) + dy};
					if (!state.isInBounds(spawnPos)) continue;
					if (!state.isWalkable(spawnPos)) continue;
					if (!state.isOccupied(spawnPos)) continue;

					fm.spawnEntity(state, targetEntityType, sourceEntity.getOwnerID(), Vector2f(spawnPos.x, spawnPos.y));
					return;
				}
			}
		}
		else
		{
			throw std::runtime_error("SpawnRandom is only available in TBS-Games");
		}
	}
	SpawnEntityRandomLocation::SpawnEntityRandomLocation(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		targetEntityTypeParam(parameters[0])
	{
	}

	void SpawnEntityRandomLocation::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if (fm.getGameType()==GameType::TBS)
		{
			
			const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);
			boost::random::uniform_int_distribution<int> widthMax(0, state.getBoardWidth());
			boost::random::uniform_int_distribution<int> heightMax(0, state.getBoardHeight());

			Vector2i spawnPos{widthMax(state.getRndEngine()), heightMax(state.getRndEngine())};
			do
			{
				spawnPos = { widthMax(state.getRndEngine())-1, heightMax(state.getRndEngine())-1 };
			} while (!state.isWalkable(spawnPos) ||!state.isOccupied(spawnPos) || !state.isInBounds(spawnPos));

			fm.spawnEntity(state, targetEntityType, -1, Vector2f(spawnPos.x, spawnPos.y));
		}
		else
		{
			throw std::runtime_error("SpawnRandom is only available in TBS-Games");
		}
	}


	SpawnEntity::SpawnEntity(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		spawnSource(parameters[0]), 
		entityTypeParam(parameters[1]), 
		targetPositionParam(parameters[2])
	{
	}

	void SpawnEntity::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& ownerID = spawnSource.getPlayerID(state, targets);
		const auto& entityType = entityTypeParam.getEntityType(state, targets);
		auto targetPosition = targetPositionParam.getPosition(state, targets);
		fm.spawnEntity(state, entityType, ownerID, targetPosition);
	}

	SpawnEntityGrid::SpawnEntityGrid(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		spawnSource(parameters[0]), 
		entityTypeParam(parameters[1]), 
		targetPositionParam(parameters[2])
	{
	}

	void SpawnEntityGrid::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& ownerID = spawnSource.getPlayerID(state, targets);
		const auto& entityType = entityTypeParam.getEntityType(state, targets);
		auto targetPosition = targetPositionParam.getPosition(state, targets);
		targetPosition.x =static_cast<int>(targetPosition.x);
		targetPosition.y =static_cast<int>(targetPosition.y);
		fm.spawnEntity(state, entityType, ownerID, targetPosition);
	}

	
	PayCostEffect::PayCostEffect(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp), 
		sourceParam(parameters[0]), 
		costParam(parameters[1])
	{
	}

	void PayCostEffect::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		//Get cost of target, parameterlist to look up and the parameters of the source
		const auto cost = costParam.getCost(state, targets);
		const auto& parameterLookUp = sourceParam.getParameterLookUp(state, targets);
		auto& parameters = sourceParam.getParameterList(state, targets);

		for (const auto& idCostPair : cost)
		{
			const auto& param = parameterLookUp.at(idCostPair.first);
			parameters[param.getIndex()] -= idCostPair.second;
		}
	}

	EquipObject::EquipObject(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{
	}

	void EquipObject::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//Equip object
		auto& entity = entityParam.getEntity(state, targets);
		auto& object = targetParam.getObject(state, targets);

		fm.executeOnEquipObjectSlot(state, entity, object);
		entity.equipObject(object.getID());
	}

	UnEquipObject::UnEquipObject(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{
	}

	void UnEquipObject::execute(GameState& state, const ForwardModel&, const std::vector<ActionTarget>& targets) const
	{
		//Equip object
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getSlotObject(state, targets);

		entity.unEquipObject(target.getID());		
	}

	UseObject::UseObject(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{
	}

	void UseObject::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//Equip object
		auto& entity = entityParam.getEntity(state, targets);
		auto& object = targetParam.getObject(state, targets);

		fm.executeOnUseObjectInventory(state, entity, object);
	}

	UseSlotObject::UseSlotObject(const std::string exp, const std::vector<FunctionParameter>& parameters)
		: Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{
	}

	void UseSlotObject::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//Equip object
		auto& entity = entityParam.getEntity(state, targets);
		auto& object = targetParam.getSlotObject(state, targets);

		fm.executeOnUseObjectSlot(state, entity, object);
	}
}


