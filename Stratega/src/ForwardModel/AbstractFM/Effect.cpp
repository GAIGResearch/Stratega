#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>

namespace SGA
{
	AddToResource::AddToResource(FunctionParameter resourceReference, FunctionParameter amount) :
		resourceReference(resourceReference),
		amount(amount)
	{

	}
	void AddToResource::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS Add to resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;
	}
	void AddToResource::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute RTS Add to resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;
	}


	void EndTurn::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS end turn" << std::endl;
		auto& casted=dynamic_cast<TBSGameState2&>(state);
		fm.endTurn(casted);
	}

	void EndTurn::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//NOT NEEDED
	}

	void Move::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute Move TBS" << std::endl;
		auto& casted = dynamic_cast<TBSGameState2&>(state);

		Entity& entity = targetToEntity(state, targets[0]);
		Vector2f& newPos = targetToPosition(state, targets[1]);
		fm.moveEntity(casted,entity,newPos);
	}

	void Move::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//NOT NEEDED
	}

	void SpawnUnit::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute Spawn Unit TBS" << std::endl;
		auto& casted = dynamic_cast<TBSGameState2&>(state);

		Entity& sourceEntity = targetToEntity(state, targets[0]);
		Vector2f& newPos = targetToPosition(state, targets[1]);

		SGA::Entity entity;
		entity.id = state.entities.size();
		entity.owner = sourceEntity.owner;
		entity.actionTypeIds.emplace_back(0);
		entity.actionTypeIds.emplace_back(1);
		entity.actionTypeIds.emplace_back(2);
		entity.position = newPos;
		entity.typeID = 0;
		entity.parameters.emplace_back(60);

		state.entities.emplace_back(entity);
	}

	void SpawnUnit::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//NOT NEEDED
	}
}
