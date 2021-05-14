#include <Stratega/Representation/Grid2D.h>

namespace SGA
{
	struct EntityPlacement
	{
		int ownerID;
		std::shared_ptr<EntityType> entityType;
		Vector2f position;
	};
	
	class LevelDefinition
	{		
	public:
		LevelDefinition(std::vector<EntityPlacement> entityPlacements, Grid2D<std::shared_ptr<TileType>> board):
			entityPlacements(entityPlacements),board(board)
		{
			
		}

		std::string name;
		std::string boardString;
		
		std::vector<EntityPlacement> entityPlacements;
		Grid2D<std::shared_ptr<TileType>> board;
	};
}