#include <ForwardModel/ActionType.h>
#include <Representation/Player.h>
#include <Representation/GameState.h>
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

	int ActionTarget::getPlayerID(const GameState& state) const
	{
		if (targetType == PlayerReference)
		{
			return data.playerID;
		}
		else if (targetType == EntityReference)
		{
			return getEntityConst(state).ownerID;
		}
		else
			return -1;
	}

	Vector2f ActionTarget::getPosition(const GameState& state) const
	{
		if (targetType == Position)
		{
			return data.position;
		}
		else if(targetType == EntityReference)
		{
			return getEntity(const_cast<GameState&>(state)).position;
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
			return state.getEntityConst(data.entityID);
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	Entity& ActionTarget::getEntity(GameState& state) const
	{
		if (targetType == Type::EntityReference)
		{
			auto* entity = state.getEntity(data.entityID);
			if (entity == nullptr)
			{
				throw std::runtime_error("A action-target contained an not existing entity.");
			}
			return *entity;
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
	}

	const EntityType& ActionTarget::getEntityType(const GameState& state) const
	{
		if (targetType == EntityTypeReference)
		{
			const auto& type = state.getEntityType(data.entityTypeID);
			return type;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}
}
