#pragma once
#include <Stratega/Representation/EntityPlacement.h>
#include <memory>
#include <Stratega/Representation/TileType.h>

namespace SGA
{
	
	class LevelDefinition
	{		
	public:
		LevelDefinition(std::vector<EntityPlacement> entityPlacements, Grid2D<std::shared_ptr<TileType>> board);

		std::string name;
		std::string boardString;
		
		std::vector<EntityPlacement> entityPlacements;
		Grid2D<std::shared_ptr<TileType>> board;
	};
}