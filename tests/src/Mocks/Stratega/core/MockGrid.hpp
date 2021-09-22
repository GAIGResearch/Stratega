#pragma once

#include "Stratega/Representation/Grid2D.h"
#include "Stratega/Representation/Tile.h"
#include "Stratega/Representation/TileType.h"
#include "gmock/gmock.h"
#include <math.h>

namespace SGA {
	class MockGrid : public Grid2D<Tile> {
		TileType fog;
	 public:
		 //MockGrid() : Grid2D(0, 0, Tile{fog,10,10}) {}

	  MOCK_METHOD(size_t, getWidth, (), (const));
	  MOCK_METHOD(size_t, getHeight, (), (const));

	};
}  