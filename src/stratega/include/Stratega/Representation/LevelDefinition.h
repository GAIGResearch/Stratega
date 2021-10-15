#pragma once
#include <Stratega/Representation/Grid2D.h>
#include <vector>
#include <Stratega/Representation/EntityPlacement.h>
#include <Stratega/Representation/TileType.h>
namespace SGA
{
	//class EntityPlacement;
	//class TileType;

	class LevelDefinition
	{		
	public:
		//LevelDefinition(std::vector<EntityPlacement> entityPlacements, Grid2D<std::shared_ptr<TileType>> board);
         LevelDefinition(
            std::vector< EntityPlacement > entityPlacements,
            Grid2D< std::shared_ptr< TileType > > board)
             : entityPlacements(entityPlacements), board(board)
         {
         }
		std::string name;
		std::string boardString;
		
		std::vector<EntityPlacement> entityPlacements;
		Grid2D<std::shared_ptr<TileType>> board;
	};
}