#include <Stratega/Agent/RuleBasedAgents/Direction.h>

namespace SGA
{
	Direction GetDirectionTo(const Vector2i& from, const Vector2i& to)
	{
		int dx = to.x - from.x;
		int dy = to.y - from.y;

		if (dx > 0)
		{
			if (dy == 0)
				return Direction::East;
			if (dy > 0)
				return Direction::SouthEast;

			return Direction::NorthEast;
		}
		if (dx < 0)
		{
			if (dy == 0)
				return Direction::West;
			if (dy > 0)
				return Direction::SouthWest;

			return Direction::NorthWest;
		}

		if (dy > 0)
			return Direction::South;
		if (dy < 0)
			return Direction::North;

		return Direction::None;
	}

	Direction ReverseDir(const Direction d)
	{
		switch (d)
		{
		case Direction::North: return Direction::South;
		case Direction::NorthEast: return Direction::SouthWest;
		case Direction::East: return Direction::West;
		case Direction::SouthEast: return Direction::NorthWest;
		case Direction::South: return Direction::North;
		case Direction::SouthWest: return Direction::NorthEast;
		case Direction::West: return Direction::East;
		case Direction::NorthWest: return Direction::SouthEast;
		case Direction::None: return Direction::None;
		}

		throw std::runtime_error("Tried reversing an unknown direction.");
	}

	Vector2i MoveTo(const Vector2i& pos, const Direction d)
	{
		switch (d)
		{
		case Direction::North: return Vector2i(pos.x, pos.y - 1);
		case Direction::NorthEast: return Vector2i(pos.x + 1, pos.y - 1);
		case Direction::East: return Vector2i(pos.x + 1, pos.y);
		case Direction::SouthEast: return Vector2i(pos.x + 1, pos.y + 1);
		case Direction::South:  return Vector2i(pos.x, pos.y + 1);
		case Direction::SouthWest: return Vector2i(pos.x - 1, pos.y + 1);
		case Direction::West: return Vector2i(pos.x - 1, pos.y);
		case Direction::NorthWest: return Vector2i(pos.x - 1, pos.y - 1);
		case Direction::None: return pos;
		}

		throw std::runtime_error("Tried moving into an unknown direction.");
	}
}