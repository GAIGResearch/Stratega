#include <Configuration/TileConfig.h>

namespace SGA
{
	TileType tileTypeFromConfig(const TileConfig& config, int id)
	{
		TileType type{};
		type.id = id;
		type.isWalkable = config.isWalkable;
		return type;
	}
}