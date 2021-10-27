#pragma once
#include <Stratega/Representation/Vector2.h>
#pragma warning( disable : 4820)
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
		/// Indicates if this tile blocks the line of sight for entities in the game. 
		/// </summary>
		bool blockSight;

		/// <summary>
		/// Indicates if this tile is walkable for units to occupy it.
		/// </summary>
		bool walkable;

		/// <summary>
		/// Position <x,y> of this tile in the board.
		/// </summary>
		Vector2i position;

	public:

		inline Tile(const TileType* newTileType,int x, int y):
			tileType(newTileType),
			blockSight(false),
			walkable(true),
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

		inline bool isWalkable() const { return walkable; }
		inline void setWalkable(bool w) { walkable = w; }

		inline bool blocksSight() const { return blockSight; }
		inline void setBlockSight(bool b) { blockSight = b; }

		inline void setPosition(int x, int y) { position = Vector2i(x,y); }
		inline int x() const { return position.x; }
		inline int y() const { return position.y; }
	};

}