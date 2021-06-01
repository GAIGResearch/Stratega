#pragma once
#include <Stratega/Representation/Vector2.h>
namespace SGA
{
	struct TileType;
	
	struct Tile
	{
		Tile(int typeID,const TileType* tileType,int x, int y):
			tileType(tileType),
			isWalkable(true),
			blocksSight(false),
			position(x, y)
		{

		}

		int getTileTypeID() const;
		
		bool isWalkable;
		bool blocksSight;
		Vector2i position;
		
	private:
		const TileType* tileType;
	};

}