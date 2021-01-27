#include <ForwardModel/ActionType.h>
#include <Representation/Player.h>
#include <Representation/GameState.h>
namespace SGA
{
	ActionTarget ActionTarget::createPositionActionTarget(Vector2f position)
	{
		return ActionTarget(Type::Position, {.position = position });
	}

	ActionTarget ActionTarget::createEntityActionTarget(int entityID)
	{
		return ActionTarget(Type::EntityReference, { .entityID = entityID });
	}

	ActionTarget ActionTarget::createPlayerActionTarget(int playerID)
	{
		return ActionTarget(Type::PlayerReference, { .playerID = playerID });
	}
	
	ActionTarget ActionTarget::createTechnologyEntityActionTarget(int technologyID)
	{
		return ActionTarget(Type::TechnologyReference, { .technologyID = technologyID });
	}

	ActionTarget ActionTarget::createContinuousActionActionTarget(int continuousActionID)
	{
		return ActionTarget(Type::ContinuousActionReference, { .continuousActionID = continuousActionID });
	}

	Vector2f ActionTarget::getPosition() const
	{
		if (targetType == Type::Position)
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

	Player& ActionTarget::getPlayer(GameState& state) const
	{
		if (targetType == Type::PlayerReference)
		{
			auto* player = state.getPlayer(data.playerID);
			if (player == nullptr)
			{
				throw std::runtime_error("A action-target contained an not existing entity.");
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
				throw std::runtime_error("A action-target contained an not existing entity.");
			}
			return *player;
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}
}
