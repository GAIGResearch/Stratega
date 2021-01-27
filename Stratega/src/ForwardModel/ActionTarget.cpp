#include <ForwardModel/ActionType.h>
#include <Representation/GameState.h>
namespace SGA
{
	ActionTarget ActionTarget::createPositionActionTarget(Vector2f position)
	{
		return ActionTarget(Position, {.position = position });
	}

	ActionTarget ActionTarget::createEntityActionTarget(int entityID)
	{
		return ActionTarget(EntityReference, { .entityID = entityID });
	}

	ActionTarget ActionTarget::createEntityTypeActionTarget(EntityTypeID entityTypeID)
	{
		return ActionTarget(EntityTypeReference, { .entityTypeID = entityTypeID });
	}

	ActionTarget ActionTarget::createTechnologyEntityActionTarget(int technologyID)
	{
		return ActionTarget(TechnologyReference, { .technologyID = technologyID });
	}

	ActionTarget ActionTarget::createContinuousActionActionTarget(int continuousActionID)
	{
		return ActionTarget(Type::ContinuousActionReference, { .continuousActionID = continuousActionID });
	}

	Vector2f ActionTarget::getPosition() const
	{
		if (targetType == Position)
		{
			return data.position;
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
			if(entity == nullptr)
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

	const EntityType& ActionTarget::getEntityType(const GameState& state) const
	{
		if(targetType == EntityTypeReference)
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
