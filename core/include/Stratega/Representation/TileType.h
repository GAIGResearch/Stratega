#pragma once
#include <Stratega/Representation/Tile.h>

namespace SGA
{
	struct TileType
	{
		std::string name;
		char symbol = '\0';
		int id = -1;
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
