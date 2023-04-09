#include <Stratega/ForwardModel/Effect.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <boost/random.hpp>

#include "Stratega/ForwardModel/FunctionParameter.h"
#pragma warning(disable: 5045)
namespace SGA
{
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
		else
		{
			fm.modifyStateParameterByIndex(state, parameterIndex, targetResource);
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
		
		if (!resourceReference.isPlayerParameter(targets))
		{
			auto& entitySource = resourceReference.getEntity(state, targets);
			fm.modifyEntityParameterByIndex(entitySource, parameterIndex, amount);
		}
		else
		{
			auto& playerSource = resourceReference.getPlayer(state, targets);
			fm.modifyPlayerParameterByIndex(playerSource, parameterIndex, amount);
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

		fm.modifyEntityParameterByIndex(entity, parameterIndex, targetResource);

		if(targetResource <= 0)
			entity.flagRemove();
	}

	Empty::Empty(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0])
	{
	}
	
	void Empty::execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const
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
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
	}

    PushUp::PushUp(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}
	
	void PushUp::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		//auto pushDir = SGA::Vector2f(+1, 0);
		auto pushDir = SGA::Vector2f(0, -1.0);
		auto newTargetPos = target.getPosition() + pushDir;
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
	}


    PushDown::PushDown(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}
	
	void PushDown::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		//auto pushDir = SGA::Vector2f(-1, 0);
		auto pushDir = SGA::Vector2f(0, 1.0);
		auto newTargetPos = target.getPosition() + pushDir;
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
	}


    PushLeft::PushLeft(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}
	
	void PushLeft::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		//auto pushDir = SGA::Vector2f(0, 1);
		auto pushDir = SGA::Vector2f(-1.0, 0.0);
		auto newTargetPos = target.getPosition() + pushDir;
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
	}


    PushRight::PushRight(const std::string exp, const std::vector<FunctionParameter>& parameters) :
		Effect(exp),
		entityParam(parameters[0]), targetParam(parameters[1])
	{

	}
	
	void PushRight::execute(GameState& state, const ForwardModel& /*fm*/, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = entityParam.getEntity(state, targets);
		auto& target = targetParam.getEntity(state, targets);

		//auto pushDir = SGA::Vector2f(0, -1);
		auto pushDir = SGA::Vector2f(1.0, 0.0);
		auto newTargetPos = target.getPosition() + pushDir;
		if (state.isWalkable(Vector2i{ static_cast<int>(newTargetPos.x), static_cast<int>(newTargetPos.y) }))
		{
			target.setPosition({ std::floor(newTargetPos.x), std::floor(newTargetPos.y) });
		}
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
			if (entity.getPath().isEmpty() || oldTargetPos.distance(targetPosition) > 0.00001)
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
			} while (!state.isWalkable(spawnPos) || !state.isInBounds(spawnPos));

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
		const auto& cost = costParam.getCost(state, targets);
		const auto& parameterLookUp = sourceParam.getParameterLookUp(state, targets);
		auto& parameters = sourceParam.getParameterList(state, targets);

		for (const auto& idCostPair : cost)
		{
			const auto& param = parameterLookUp.at(idCostPair.first);
			parameters[static_cast<size_t>(param.getIndex())] -= idCostPair.second;
		}
	}
}
