#pragma once
#include <Stratega/Representation/Vector2.h>
namespace SGA
{
	struct TileType;
	
	struct Tile
	{

	private:

		/// <summary>
		/// Type of tile
		/// </summary>
		const TileType* tileType;

		/// <summary>
		/// Indicates if this tile is walkable for units to occupy it.
		/// </summary>
		bool walkable;

		/// <summary>
		/// Indicates if this tile blocks the line of sight for entities in the game. 
		/// </summary>
		bool blockSight;

		/// <summary>
		/// Position <x,y> of this tile in the board.
		/// </summary>
		Vector2i position;

	public:

		Tile(int typeID,const TileType* tileType,int x, int y):
			tileType(tileType),
			walkable(true),
			blockSight(false),
			position(x, y)
		{
		}

		/// <summary>
		/// Returns the tile type ID of this tile
		/// </summary>
		/// <returns>type ID of this tile</returns>
		int getTileTypeID() const;
		
		/// <summary>
		/// Returns the name of this tile type
		/// </summary>
		/// <returns>Name ot the tile type of this type.</returns>
		const std::string name() const;
		
		/*Getters and setters for Tile*/

		bool isWalkable() const { return walkable; }
		void setWalkable(bool w) { walkable = w; }

		bool blocksSight() const { return blockSight; }
		void setBlockSight(bool b) { blockSight = b; }

		void setPosition(int x, int y) { position = Vector2i(x,y); }
		int x() const { return position.x; }
		int y() const { return position.y; }
	};

}