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

		Tile toTile() const
		{
			Tile instance(id, 0, 0);
			instance.isWalkable = isWalkable;
			return instance;
		}
	};
}
