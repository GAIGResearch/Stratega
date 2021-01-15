#include <ForwardModel/ActionType.h>
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

	ActionTarget ActionTarget::createTechnologyEntityActionTarget(int technologyID)
	{
		return ActionTarget(Type::TechnologyReference, { .technologyID = technologyID });
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
			return state.getEntity(data.entityID);
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
	}

	//
//	Vector2f targetToPosition(const GameState& state, const ActionTarget& target)
//	{
//		if(std::holds_alternative<int>(target))
//		{
//			return targetToEntity(state, target).position;
//		}
//		
//		return std::get<Vector2f>(target);
//	}
//	
//	const Entity& targetToEntity(const GameState& state, const ActionTarget& target)
//	{
//		return state.getEntityConst(std::get<int>(target));
//	}
//
//	 Entity& targetToEntity( GameState& state,  const ActionTarget& target)
//	{
//		return state.getEntity(std::get<int>(target));
//	}
}
