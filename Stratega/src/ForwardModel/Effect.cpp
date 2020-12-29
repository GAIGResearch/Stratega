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
		std::cout << "Execute TBS Add to resource" << std::endl;
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
	
	void Attack::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		if(const auto* tbsFM = dynamic_cast<const TBSForwardModel*>(&fm))
		{
			auto& entity = resourceReference.getEntity(state, targets);
			auto& targetResource = resourceReference.getParameter(state, targets);
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
			std::cout << "Execute Move TBS" << std::endl;
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
	{	
	}

	void SpawnUnit::execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute Spawn Unit TBS" << std::endl;

		Entity& sourceEntity = targetToEntity(state, targets[0]);
		auto newPos = targetToPosition(state, targets[1]);

		// ToDo Remove hardcoded stuff
		Entity entity;
		entity.id = state.entities.size();
		entity.ownerID = sourceEntity.ownerID;
		entity.attachedActions.emplace_back(Entity::ActionInfo{ 2, 0 });
		entity.position = newPos;
		entity.typeID = 0;
		entity.parameters.emplace_back(60);
		entity.parameters.emplace_back(0);

		state.entities.emplace_back(entity);
	}
}
