#pragma once
#include <Representation/Tile.h>

namespace SGA
{
	struct TileType
	{
		std::string name;
		char symbol;
		int id;
		bool isWalkable = true;
		bool blocksSight = false;
		bool isDefaultTile = false;

		Tile toTile(int x, int y) const
		{
			Tile instance(id, x, y);
			instance.isWalkable = isWalkable;
			instance.blocksSight = blocksSight;
			return instance;
		}
	};
}
