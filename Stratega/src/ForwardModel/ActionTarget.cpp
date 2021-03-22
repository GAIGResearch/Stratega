#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameState.h>
namespace SGA
{
	ActionTarget ActionTarget::createPositionActionTarget(Vector2f position)
	{
		return ActionTarget(Position, { .position = position });
	}

	ActionTarget ActionTarget::createEntityActionTarget(int entityID)
	{
		return ActionTarget(EntityReference, { .entityID = entityID });
	}

	ActionTarget ActionTarget::createEntityTypeActionTarget(EntityTypeID entityTypeID)
	{
		return ActionTarget(EntityTypeReference, { .entityTypeID = entityTypeID });
	}

	ActionTarget ActionTarget::createTileTypeActionTarget(EntityTypeID tileTypeID)
	{
		return ActionTarget(TileTypeReference, { .tileTypeID = tileTypeID });
	}
	
	ActionTarget ActionTarget::createPlayerActionTarget(int playerID)
	{
		return ActionTarget(Type::PlayerReference, { .playerID = playerID });
	}

	ActionTarget ActionTarget::createTechnologyEntityActionTarget(int technologyID)
	{
		return ActionTarget(TechnologyReference, { .technologyID = technologyID });
	}

	ActionTarget ActionTarget::createContinuousActionActionTarget(int continuousActionID)
	{
		return ActionTarget(Type::ContinuousActionReference, { .continuousActionID = continuousActionID });
	}

	int ActionTarget::getPlayerID() const
	{
		if (targetType == PlayerReference)
		{
			return data.playerID;
		}

		throw std::runtime_error("Type not recognised");
	}

	int ActionTarget::getPlayerID(const GameState& state) const
	{
		if (targetType == PlayerReference)
		{
			return data.playerID;
		}
		else if(targetType == EntityReference)
		{
			return state.getEntityConst(data.entityID)->ownerID;
		}

		throw std::runtime_error("Type not recognised");
	}
	
	const std::unordered_set<EntityTypeID>& ActionTarget::getSpawneableEntities(const GameState& state) const
	{
		if (targetType == PlayerReference)
		{
			return *state.playerSpawnableTypes;
		}
		else if (targetType == EntityReference)
		{
			return state.getEntityType(getEntityConst(state).typeID).spawnableEntityTypes;
		}

		throw std::runtime_error("Type not recognised");
	}

	Vector2f ActionTarget::getPosition(const GameState& state) const
	{
		if (targetType == Position)
		{
			return data.position;
		}
		else if(targetType == EntityReference)
		{
			return getEntity(const_cast<GameState&>(state))->position;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	const Entity& ActionTarget::getEntityConst(const GameState& state) const
	{
		if (targetType == Type::EntityReference)
		{
			return *state.getEntityConst(data.entityID);
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	Entity* ActionTarget::getEntity(GameState& state) const
	{
		if (targetType == Type::EntityReference)
		{
			auto* entity = state.getEntity(data.entityID);
		
			return entity;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	Player& ActionTarget::getPlayer(GameState& state) const
	{
		if (targetType == Type::PlayerReference)
		{
			auto* player = state.getPlayer(data.playerID);
			if (player == nullptr)
			{
				throw std::runtime_error("A action-target contained an not existing player.");
			}
			return *player;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	const Player& ActionTarget::getPlayerConst(const GameState& state) const
	{
		if (targetType == Type::PlayerReference)
		{
			auto* player = state.getPlayer(data.playerID);
			if (player == nullptr)
			{
				throw std::runtime_error("A action-target contained an not existing player.");
			}
			return *player;
		}

		throw std::runtime_error("Type not recognised");
	}

	const EntityType& ActionTarget::getEntityType(const GameState& state) const
	{
		if (targetType == EntityTypeReference)
		{
			const auto& type = state.getEntityType(data.entityTypeID);
			return type;
		}
		else if(targetType == EntityReference)
		{
			const auto& type = state.getEntityType(state.getEntityConst(data.entityID)->typeID);
			return type;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	const TileType& ActionTarget::getTileType(const GameState& state) const
	{
		if (targetType == TileTypeReference)
		{
			const auto& type = state.getTileType(data.entityTypeID);
			return type;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	bool ActionTarget::isValid(const GameState& state) const
	{
		switch (targetType)
		{
		case EntityReference:
			return state.getEntityConst(data.entityID);
			break;
		default: return true;
		}
	}
		
	bool ActionTarget::isValid(const GameState& state,const  ActionType& actionType, const std::vector<ActionTarget>& actionTargets)
	{
		auto isValid = true;
		for (size_t i = 0; i < actionType.actionTargets.size(); ++i)		
		{
			//Check valid targets
			if (!actionType.actionTargets[i].first.isValid(state, actionTargets[i + 1], actionTargets[0]))
				return false;
			
			
			for (const auto& condition : actionType.actionTargets[i].second)
			{
				if (!condition->isFullfilled(state, actionTargets))
					isValid = false;
			}
		}
		
		return isValid;
	}
}
