#include <Representation/GameState.h>
#include <ForwardModel/Condition.h>

namespace SGA
{
	bool GameState::canExecuteAction(Entity& entity, ActionType& actionType)
	{
		//Check preconditions
		for (const auto& precondition : actionType.preconditions)
		{
			if (!precondition->isFullfilled(*this, { entity.id }))
			{
				return false;
			}
		}

		return true;
	}
}
