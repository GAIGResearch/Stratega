#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <ForwardModel/AbstractFM/RTSAbstractForwardModel.h>

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
		/*entityTarget(parameters.at(0)),*/
		resourceReference(parameters.at(0)),
		amount(parameters.at(1))
	{

	}
	void Attack::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS Attack" << std::endl;

				
		auto& entity = resourceReference.getEntity(state, targets);
		
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

		Entity& unit = targetToEntity(state, targets[0]);
		Vector2f& targetPos = targetToPosition(state, targets[1]);
		
		
		//Get end position of current path
		Vector2f oldTargetPos(0, 0);
		oldTargetPos.x = unit.path.m_straightPath[(unit.path.m_nstraightPath - 1) * 3];
		oldTargetPos.y = unit.path.m_straightPath[((unit.path.m_nstraightPath - 1) * 3) + 2];

		//Check if path is empty or is a diferent path to the target pos
		if (unit.path.m_nstraightPath == 0 || targetPos != oldTargetPos) {
			auto& casted = dynamic_cast<RTSGameState2&>(state);
			Path path = fm.findPath(casted, unit.position, targetPos);
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
		auto movementSpeed = unit.movementSpeed * fm.deltaTime;
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

	SpawnUnit::SpawnUnit(const std::vector<FunctionParameter>& parameters)
	{
		
	}

	void SpawnUnit::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute Spawn Unit TBS" << std::endl;
		
		Entity& sourceEntity = targetToEntity(state, targets[0]);
		Vector2f& newPos = targetToPosition(state, targets[1]);

		SGA::Entity entity;
		entity.id = state.entities.size();
		entity.owner = sourceEntity.owner;
		//entity.actionTypeIds.emplace_back(1);
		entity.actionTypeIds.emplace_back(2);
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
