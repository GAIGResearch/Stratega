#include <Stratega/Representation/Grid2D.h>

namespace SGA
{

	struct TileType;
	struct EntityType;

	struct EntityPlacement
	{
		int ownerID;
		std::shared_ptr<EntityType> entityType;
		Vector2f position;
	};
}