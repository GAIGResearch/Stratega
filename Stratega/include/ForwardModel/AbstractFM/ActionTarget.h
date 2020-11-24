#pragma once
#include <variant>
#include <Representation/Vector2.h>

namespace SGA
{
	struct GameState;
	struct Entity;
	
	typedef std::variant<int, Vector2f> ActionTarget;

	Vector2f targetToPosition(const GameState& state, const ActionTarget& target);
	Entity& targetToEntity(const GameState& state, const ActionTarget& target);
}