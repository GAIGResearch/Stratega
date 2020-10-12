#pragma once
#include <vector>
#include <Representation/Tile.h>
#include <Representation/TileType.h>

namespace SGA
{
	struct TileType;

	class Board
	{
	public:
		Board(int width, int height)
			:width(width), height(height)
		{
			for (auto x = 0; x < width; ++x)
			{
				for (auto y = 0; y < height; ++y)
				{
					tiles.push_back((Tile(0, x, y)));
				}	
			}
		}

		Tile& setTile(const TileType& type, int x, int y)
		{
			auto& tileRef = getTile(x, y);
			tileRef = type.toTile();
			tileRef.position = { x, y };
			return tileRef;
			
		}
		
		Tile& getTile(int x, int y)
		{
			return tiles[y * width + x];
		}

		int getWidth() const
		{
			return width;
		}
		 
		int getHeight() const
		{
			return height;
		}
		
	private:
		int width;
		int height;

		std::vector<Tile> tiles;
	};
}