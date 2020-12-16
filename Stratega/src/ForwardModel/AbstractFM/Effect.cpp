#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>

namespace SGA
{
	AddToResource::AddToResource(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
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

	Attack::Attack(const std::vector<FunctionParameter>& parameters) :
		entityTarget(parameters.at(0)),
		resourceReference(parameters.at(1)),
		amount(parameters.at(2))
	{

	}
	void Attack::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS Attack" << std::endl;

				
		auto& entity = entityTarget.getEntity(state, targets);
		
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource -= amount;
		if (targetResource <= 0)
			entity.shouldRemove = true;
	}

	void Attack::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		
	}
	
	RemoveFromResource::RemoveFromResource(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
	{

	}
	void RemoveFromResource::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS Remove from resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource -= amount;
	}

	void RemoveFromResource::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		/*std::cout << "Execute RTS Add to resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;*/
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
		//entity.actionTypeIds.emplace_back(1);
		entity.actionTypeIds.emplace_back(2);
		entity.actionTypeIds.emplace_back(4);
		entity.position = newPos;
		entity.typeID = 0;
		entity.parameters.emplace_back(60);
		entity.parameters.emplace_back(0);

		state.entities.emplace_back(entity);
	}

	void SpawnUnit::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//NOT NEEDED
	}
}
