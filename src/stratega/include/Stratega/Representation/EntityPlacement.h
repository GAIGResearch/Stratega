#pragma once
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/EntityType.h>
#include <memory>

namespace SGA
{

	struct EntityPlacement
	{
		int ownerID;
		std::shared_ptr<EntityType> entityType;
		Vector2f position;
	};
}