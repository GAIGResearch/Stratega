#include <Representation/GameState.h>
#include <ForwardModel/Condition.h>

namespace SGA
{
	bool GameState::canExecuteAction(Entity& entity, ActionType& actionType)
	{
		//Check preconditions
		for (const auto& precondition : actionType.preconditions)
		{
			if (!precondition->isFullfilled(*this, { ActionTarget::createEntityActionTarget(entity.id) }))
			{
				return false;
			}
		}

		return true;
	}

	const Entity* GameState::getEntityAt(const Vector2f& pos) const
	{
		for(const auto& entity : entities)
		{
			if(static_cast<int>(pos.x) == static_cast<int>(entity.position.x) && static_cast<int>(pos.y) == static_cast<int>(entity.position.y))
			{
				return &entity;
			}
		}

		return nullptr;
	}
}
