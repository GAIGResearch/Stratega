#pragma once
#pragma once
#include <vector>
#include <queue>
#include <map>
#include <Stratega/Representation/Vector2.h>

namespace SGA
{
	enum class Direction
	{
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest,
		None
	};

	const Direction CARDINAL_DIRECTIONS[4] = { Direction::North, Direction::East, Direction::South, Direction::West };
	const Direction ALL_DIRECTIONS[8] = { Direction::North, Direction::NorthEast, Direction::East, Direction::SouthEast, Direction::South, Direction::SouthWest, Direction::West, Direction::NorthWest };

	Direction GetDirectionTo(const Vector2i& from, const Vector2i& to);
	Direction ReverseDir(Direction d);
	Vector2i MoveTo(const Vector2i& pos, Direction d);
}