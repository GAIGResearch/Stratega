#pragma once
#include <Representation/Vector2.h>
namespace SGA
{
	struct Tile
	{
		Tile(int typeID, int x, int y):
			tileTypeID(typeID),
			isWalkable(true),
			position(x, y)
		{

		}

		int tileTypeID;
		bool isWalkable;
		Vector2i position;
	};

}