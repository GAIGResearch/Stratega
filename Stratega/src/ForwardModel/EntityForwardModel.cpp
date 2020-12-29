#include <ForwardModel/EntityForwardModel.h>

namespace SGA
{
	bool EntityForwardModel::canPlayerPlay(Player& player) const
	{
		if (player.state.get().fogOfWarId != -1 && player.id != player.state.get().fogOfWarId)
			return true;

		switch (winCondition)
		{
		case WinConditionType::UnitAlive:
		{
			//player.getEntities();
			bool hasKing = false;
			std::vector<Entity*> units = player.getEntities();

			for (auto& unit : units)
			{
				//Check if player has units
				if (unit->typeID == targetUnitTypeID)
				{
					hasKing = true;
				}
			}

			if (!hasKing)
			{
				return false;
			}

			break;
		}
		case WinConditionType::LastManStanding:
		{
			std::vector<Entity*> units = player.getEntities();

			if (units.empty())
			{
				return false;
			}
			break;
		}
		}

		return true;
	}

	void EntityForwardModel::executeAction(GameState& state, const Action& action) const
	{
		action.execute(state, *this);

		auto& actionType = state.getActionType(action.actionTypeID);
		if(actionType.sourceType == ActionSourceType::Unit)
		{
			// Remember when the action was executed
			auto& executingEntity = targetToEntity(state, action.targets[0]);
			// ToDo We should probably find a way to avoid this loop
			for(auto& actionInfo : executingEntity.attachedActions)
			{
				if(actionInfo.actionTypeID == action.actionTypeID)
				{
					actionInfo.lastExecutedTick = state.currentTick;
					break;
				}
			}
		}
	}

	void EntityForwardModel::endTick(GameState& state) const
	{
		state.currentTick++;
	}
}
