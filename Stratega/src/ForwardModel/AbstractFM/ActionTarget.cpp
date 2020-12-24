#include <ForwardModel/AbstractFM/ActionType.h>
#include <Representation/AbstractGS/GameState.h>
namespace SGA
{
	Vector2f targetToPosition(const GameState& state, const ActionTarget& target)
	{
		return std::get<Vector2f>(target);
	}
	
	const Entity& targetToEntity(const GameState& state, const ActionTarget& target)
	{
		return state.getEntityConst(std::get<int>(target));
	}

	 Entity& targetToEntity( GameState& state,  const ActionTarget& target)
	{
		return state.getEntity(std::get<int>(target));
	}
}