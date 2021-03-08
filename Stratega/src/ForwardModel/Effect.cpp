#include <Stratega/ForwardModel/Effect.h>
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>

namespace SGA
{
	ModifyResource::ModifyResource(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
	{

	}
	
	void ModifyResource::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& targetResource = resourceReference.getParameterValue(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;
	}

	Attack::Attack(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
	{

	}
	
	void Attack::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{		
		auto& entity = resourceReference.getEntity(state, targets);
		auto& targetResource = resourceReference.getParameterValue(state, targets);
		auto amount = this->amount.getConstant(state, targets);

		targetResource -= amount;
		if (targetResource <= 0)
			entity.shouldRemove = true;
	}

	Move::Move(const std::vector<FunctionParameter>& parameters)
		: entityParam(parameters[0]), targetPositionParam(parameters[1])
	{
	}
	
	void Move::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& entity = entityParam.getEntity(state, targets);
		auto targetPosition = targetPositionParam.getPosition(state, targets);
		if (const auto* tbsFM = dynamic_cast<const TBSForwardModel*>(&fm))
		{
			entity.position = { std::floor(targetPosition.x), std::floor(targetPosition.y) };
		}
		else if(const auto* rtsFM = dynamic_cast<const RTSForwardModel*>(&fm))
		{
			//Get end position of current path
			Vector2f oldTargetPos;
			oldTargetPos.x = entity.path.m_straightPath[(entity.path.m_nstraightPath - 1) * 3];
			oldTargetPos.y = entity.path.m_straightPath[((entity.path.m_nstraightPath - 1) * 3) + 2];

			// Compute a new path if the entity doesn't have one or the new target is different from the old one
			if (entity.path.isEmpty() || oldTargetPos.distance(targetPosition) > 0.00001)
			{
				const auto& rtsState = dynamic_cast<RTSGameState&>(state);
				Path path = rtsFM->findPath(rtsState, entity.position, targetPosition);
				entity.path = path;
				entity.path.currentPathIndex++;
			}
		}
	}

	SetToMaximum::SetToMaximum(const std::vector<FunctionParameter>& parameters)
		: targetResource(parameters[0])
	{
	}

	void SetToMaximum::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& param = targetResource.getParameter(state, targets);
		auto& paramValue = targetResource.getParameterValue(state, targets);

		paramValue = param.maxValue;
	}

	TransferEffect::TransferEffect(const std::vector<FunctionParameter>& parameters)
		: sourceParam(parameters[0]), targetParam(parameters[1]), amountParam(parameters[2])
	{
	}

	void TransferEffect::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& sourceType = sourceParam.getParameter(state, targets);
		const auto& targetType = targetParam.getParameter(state, targets);
		auto& sourceValue = sourceParam.getParameterValue(state, targets);
		auto& targetValue = targetParam.getParameterValue(state, targets);
		auto amount = amountParam.getConstant(state, targets);

		// Compute how much the source can transfer, if the source does not have enough just take everything
		amount = std::min(amount, sourceValue - sourceType.minValue);
		// Transfer
		sourceValue -= amount;
		// ToDo should check the maximum, but currently we have no way to set the maximum in the configuration
		// Resulting in problems for ProtectTheBase
		targetValue = targetValue + amount;
	}

	ChangeOwnerEffect::ChangeOwnerEffect(const std::vector<FunctionParameter>& parameters)
		: targetEntityParam(parameters[0]), playerParam(parameters[1])
	{
	}
	
	void ChangeOwnerEffect::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& targetEntity = targetEntityParam.getEntity(state, targets);
		auto& newOwner = playerParam.getPlayer(state, targets);
		targetEntity.ownerID = newOwner.id;
	}

	RemoveEntityEffect::RemoveEntityEffect(const std::vector<FunctionParameter>& parameters)
		: targetEntityParam(parameters[0])
	{
	}

	void RemoveEntityEffect::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		auto& targetEntity = targetEntityParam.getEntity(state, targets);
		targetEntity.shouldRemove = true;
	}

	ResearchTechnology::ResearchTechnology(const std::vector<FunctionParameter>& parameters)
		: playerParam(parameters[0]),
		  technologyTypeParam(parameters[1])
	{
	}

	void ResearchTechnology::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& targetPlayer = playerParam.getPlayer(state, targets);
		state.technologyTreeCollection->researchTechnology(targetPlayer.id, targets[1].getTechnologyID());
	}

	SpawnEntityRandom::SpawnEntityRandom(const std::vector<FunctionParameter>& parameters)
		: sourceEntityParam(parameters[0]), targetEntityTypeParam(parameters[1])
	{
	}

	void SpawnEntityRandom::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if (const auto* tbsFM = dynamic_cast<const TBSForwardModel*>(&fm))
		{
			auto& sourceEntity = sourceEntityParam.getEntity(state, targets);
			const auto& targetEntityType = targetEntityTypeParam.getEntityType(state, targets);

			for(int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					Vector2i spawnPos{ static_cast<int>(sourceEntity.position.x) + dx, static_cast<int>(sourceEntity.position.y) + dy};
					if (!state.isInBounds(spawnPos)) continue;
					if (!state.isWalkable(spawnPos)) continue;

					fm.spawnEntity(state, targetEntityType, sourceEntity.ownerID, spawnPos);
					return;
				}
			}
		}
		else
		{
			throw std::runtime_error("SpawnRandom is only available in TBS-Games");
		}
	}


	SpawnEntity::SpawnEntity(const std::vector<FunctionParameter>& parameters)
		: spawnSource(parameters[0]), entityTypeParam(parameters[1]), targetPositionParam(parameters[2])
	{
	}

	void SpawnEntity::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		const auto& ownerID = spawnSource.getPlayerID(state, targets);
		const auto& entityType = entityTypeParam.getEntityType(state, targets);
		auto targetPosition = targetPositionParam.getPosition(state, targets);
		fm.spawnEntity(state, entityType, ownerID, targetPosition);
	}

	
	PayCostEffect::PayCostEffect(const std::vector<FunctionParameter>& parameters)
		: sourceParam(parameters[0]), costParam(parameters[1])
	{
	}

	void PayCostEffect::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//Get cost of target, parameterlist to look up and the parameters of the source
		const auto& cost = costParam.getCost(state, targets);
		const auto& parameterLookUp = sourceParam.getParameterLookUp(state, targets);
		auto& parameters = sourceParam.getParameterList(state, targets);

		for (const auto& idCostPair : cost)
		{
			const auto& param = parameterLookUp.at(idCostPair.first);
			parameters[param.index] -= idCostPair.second;
		}
	}
}
