#include <ForwardModel/EntityForwardModel.h>

namespace SGA
{
	bool EntityForwardModel::canPlayerPlay(const GameState& state, Player& player) const
	{
		if (state.fogOfWarId != -1 && player.id != state.fogOfWarId)
			return true;

		switch (winCondition)
		{
		case WinConditionType::UnitAlive:
		{
			//player.getEntities();
			bool hasKing = false;
			for (auto& unit : state.getPlayerEntities(player.id))
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
			if (state.getPlayerEntities(player.id).empty())
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
			auto& executingEntity = action.targets[0].getEntity(state);
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

		// Execute OnTick-trigger
		for(const auto& onTickEffect : onTickEffects)
		{		
			for (const auto& entity : state.entities)
			{
				if (onTickEffect.validTargets.find(entity.typeID) == onTickEffect.validTargets.end())
					continue;
				std::vector<ActionTarget> targets;
				targets.emplace_back(ActionTarget::createEntityActionTarget(entity.id));
				auto isValid = true;
				for(const auto& condition : onTickEffect.conditions)
				{
					if(!condition->isFullfilled(state, targets))
					{
						isValid = false;
						break;
					}
				}

				if(isValid)
				{
					for (const auto& effect : onTickEffect.effects)
					{
						effect->execute(state, *this, targets);
					}
				}
			}
		}
	}

	void EntityForwardModel::spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const
	{
		auto entityID = state.addEntity(entityType, playerID, position);

		std::vector<ActionTarget> targets = { entityID };
		for(const auto& onSpawnEffect : onEntitySpawnEffects)
		{
			if (onSpawnEffect.validTargets.find(entityType.id) == onSpawnEffect.validTargets.end())
				continue;

			auto isValid = true;
			for (const auto& condition : onSpawnEffect.conditions)
			{
				if (!condition->isFullfilled(state, targets))
				{
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				for (const auto& effect : onSpawnEffect.effects)
				{
					effect->execute(state, *this, targets);
				}
			}
		}
	}
}
