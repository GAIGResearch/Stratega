#pragma once
#include <Representation/Tile.h>

namespace SGA
{
	struct TileType
	{
		int id;
		bool isWalkable;

		Tile toTile() const
		{
			Tile instance(id, 0, 0);
			instance.isWalkable = isWalkable;
			return instance;
		}
	};
}
