#pragma once
#include <Stratega/Representation/Tile.h>

namespace SGA
{
	struct TileType
	{
		/// <summary>
		/// Name of the tile as given in YAML>
		/// </summary>
		std::string name;

		/// <summary>
		/// Symbol of this tile as used in YAML for level placement.
		/// </summary>
		char symbol = '\0';

		/// <summary>
		/// Unique ID for this tile type.
		/// </summary>
		int id = -1;

		/// <summary>
		/// Indicates if the tile is walkable by units
		/// </summary>
		bool isWalkable = true;

		/// <summary>
		/// Indicates if this tile blocks the line of sight
		/// </summary>
		bool blocksSight = false;

		/// <summary>
		/// Indicates if this is a default tile. Default tiles are used for fog of war (to substitute
		///  elements that should be hidden) and to be place in positions where units are defined in the YAML level layout.
		/// </summary>
		bool isDefaultTile = false;

		/// <summary>
		/// Creates an instantiation of this tile given a position
		/// </summary>
		/// <param name="x">X position of the tile</param>
		/// <param name="y">Y position of the tile</param>
		/// <returns>A Tile object of this type in the position <x,y></returns>
		Tile toTile(int x, int y) const
		{
			Tile instance(id, this, x, y);
			instance.isWalkable = isWalkable;
			instance.blocksSight = blocksSight;
			return instance;
		}
	};
}
