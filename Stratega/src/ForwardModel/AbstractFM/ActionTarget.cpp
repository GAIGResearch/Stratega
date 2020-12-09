#include <ForwardModel/AbstractFM/ActionType.h>
#include <Representation/AbstractGS/GameState.h>
namespace SGA
{
	Vector2f targetToPosition(const GameState& state, const ActionTarget& target)
	{
		return Vector2f();
	}
	
	const Entity& targetToEntity(const GameState& state, const ActionTarget& target)
	{
		return state.getEntityConst(std::get<int>(target));
	}
}
