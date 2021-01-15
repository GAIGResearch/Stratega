#include <ForwardModel/Effect.h>
#include <ForwardModel/EntityForwardModel.h>
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>

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
		/*entityTarget(parameters.at(0)),*/
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
	{

	}
	
	void Attack::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if(const auto* tbsFM = dynamic_cast<const TBSForwardModel*>(&fm))
		{
			auto& entity = resourceReference.getEntity(state, targets);
			auto& targetResource = resourceReference.getParameterValue(state, targets);
			auto amount = this->amount.getConstant(state, targets);

			targetResource -= amount;
			if (targetResource <= 0)
				entity.shouldRemove = true;
		}
	}
	
	void Move::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if (const auto* tbsFM = dynamic_cast<const TBSForwardModel*>(&fm))
		{
			auto& tbsState = dynamic_cast<TBSGameState&>(state);
			auto& entity = targetToEntity(state, targets[0]);
			auto newPos = targetToPosition(state, targets[1]);
			tbsFM->moveEntity(tbsState, entity, newPos);
		}
		else if(const auto* rtsFM = dynamic_cast<const RTSForwardModel*>(&fm))
		{
			Entity& unit = targetToEntity(state, targets[0]);
			Vector2f targetPos = targetToPosition(state, targets[1]);

			//Get end position of current path
			Vector2f oldTargetPos(0, 0);
			oldTargetPos.x = unit.path.m_straightPath[(unit.path.m_nstraightPath - 1) * 3];
			oldTargetPos.y = unit.path.m_straightPath[((unit.path.m_nstraightPath - 1) * 3) + 2];

			//Check if path is empty or is a diferent path to the target pos
			if (unit.path.m_nstraightPath == 0 || targetPos != oldTargetPos)
			{
				const auto& rtsState = dynamic_cast<RTSGameState&>(state);
				Path path = rtsFM->findPath(rtsState, unit.position, targetPos);
				unit.path = path;
				unit.path.currentPathIndex++;
			}

			//Check if path has points to visit
			if (unit.path.m_nstraightPath > 0)
			{
				//Assign the current path index as target
				targetPos = Vector2f(unit.path.m_straightPath[unit.path.currentPathIndex * 3], unit.path.m_straightPath[unit.path.currentPathIndex * 3 + 2]);
			}

			auto movementDir = targetPos - unit.position;
			auto movementDistance = movementDir.magnitude();
			auto movementSpeed = unit.movementSpeed * rtsFM->deltaTime;
			if (movementDistance <= movementSpeed)
			{
				unit.path.currentPathIndex++;
				if (unit.path.m_nstraightPath <= unit.path.currentPathIndex)
				{
					if (movementDistance <= movementSpeed) {
						unit.position = targetPos;
						//unit.executingAction.type = RTSActionType::None;
						unit.executingAction = Action();
						unit.path = Path();
					}
				}
			}
			else
			{
				unit.position = unit.position + (movementDir / movementDir.magnitude()) * movementSpeed;
			}
		}
	}

	SpawnUnit::SpawnUnit(const std::vector<FunctionParameter>& parameters)
		: entityTypeParam(parameters[0]), targetPositionParam(parameters[1])
	{	
	}

	void SpawnUnit::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		// ToDo Is there a better way to do this?
		int playerID = -1;
		if(std::holds_alternative<int>(targets[0]))
		{
			auto& executingEntity = targetToEntity(state, targets[0]);
			playerID = executingEntity.ownerID;
		}
		
		const auto& entityType = entityTypeParam.getEntityType(state, targets);
		state.addEntity(entityType, playerID, targetPositionParam.getPosition(state, targets));
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


}
