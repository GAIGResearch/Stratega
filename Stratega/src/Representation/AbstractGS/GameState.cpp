#include <Representation/AbstractGS/GameState.h>
#include <ForwardModel/AbstractFM/Condition.h>

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
