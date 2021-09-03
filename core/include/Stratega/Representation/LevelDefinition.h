#include <Stratega/Representation/EntityPlacement.h>
#include <memory>

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