#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/TileType.h>
namespace SGA
{
	int Tile::getTileTypeID() const
	{
		int tileTypeID = -1;
		if (tileType)
			tileTypeID = tileType->getID();

		return tileTypeID;
	}


	const std::string Tile::name() const {
	
		if(tileType != nullptr)
			return tileType->getName();
		return "Fog";
	}

}