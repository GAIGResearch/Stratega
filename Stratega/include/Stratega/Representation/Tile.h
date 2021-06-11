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

		/// <summary>
		/// Returns the tile type ID of this tile
		/// </summary>
		/// <returns>type ID of this tile</returns>
		int getTileTypeID() const;
		
		/// <summary>
		/// Indicates if this tile is walkable for units to occupy it.
		/// </summary>
		bool isWalkable;

		/// <summary>
		/// Indicates if this tile blocks the line of sight for entities in the game. 
		/// </summary>
		bool blocksSight;

		/// <summary>
		/// Position <x,y> of this tile in the board.
		/// </summary>
		Vector2i position;
		
	private:

		/// <summary>
		/// Type of tile
		/// </summary>
		const TileType* tileType;
	};

}