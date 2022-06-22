#include "headers.h"

namespace stratega
{
	void tile(py::module_& m)
	{
		// ---- TileType ----
		py::class_<SGA::TileType, std::shared_ptr<SGA::TileType>>(m, "TileType", "Contains all the information of a tile type. This information includes a list of parameters definitions, if a tile blocks sight or if is wakable.")
			.def("to_tile", &SGA::TileType::toTile, py::arg("x"), py::arg("y"), "Creates an instantiation of this tile given a position")

			.def_property("name", &SGA::TileType::getName, &SGA::TileType::setName)
			.def("get_name", &SGA::TileType::getName)
			.def("set_name", &SGA::TileType::setName, py::arg("name"))

			.def_property("symbol", &SGA::TileType::getSymbol, &SGA::TileType::setSymbol)
			.def("get_symbol", &SGA::TileType::getSymbol)
			.def("set_symbol", &SGA::TileType::setSymbol, py::arg("c"))

			.def_property("id", &SGA::TileType::getID, &SGA::TileType::setID)
			.def("get_id", &SGA::TileType::getID)
			.def("set_id", &SGA::TileType::setID, py::arg("id"))

			.def_property("walkable", &SGA::TileType::isWalkable, &SGA::TileType::setWalkable)
			.def("is_walkable", &SGA::TileType::isWalkable)
			.def("set_walkable", &SGA::TileType::setWalkable, py::arg("w"))

			.def_property("block_sight", &SGA::TileType::blockSight, &SGA::TileType::setBlockSight)
			.def("get_block_sight", &SGA::TileType::blockSight)
			.def("set_block_sight", &SGA::TileType::setBlockSight, py::arg("b"))

			.def_property("default_tile", &SGA::TileType::isDefaultTile, &SGA::TileType::setDefaultTile)
			.def("is_default_tile", &SGA::TileType::isDefaultTile)
			.def("set_default_tile", &SGA::TileType::setDefaultTile, py::arg("defaultTile"))
			;

		// ---- Tile ----
		py::class_<SGA::Tile>(m, "Tile", "Contains the instanted object of a Tile information.")
			.def(py::init<const SGA::TileType*, int, int>(), py::arg("tileType"), py::arg("x"), py::arg("y"))
			.def("get_tile_type_id", &SGA::Tile::getTileTypeID, "Returns the tile type ID of this tile")
			.def("name", &SGA::Tile::name, "Returns the tile name")

			.def_property("walkable", &SGA::Tile::isWalkable, &SGA::Tile::setWalkable)
			.def("is_walkable", &SGA::Tile::isWalkable)
			.def("set_walkable", &SGA::Tile::setWalkable, py::arg("w"))

			.def_property("blocks_sight", &SGA::Tile::blocksSight, &SGA::Tile::setBlockSight)
			.def("get_blocks_sight", &SGA::Tile::blocksSight)
			.def("set_block_sight", &SGA::Tile::setBlockSight, py::arg("b"))

			.def("set_position", &SGA::Tile::setPosition, py::arg("x"), py::arg("y"))

			.def("x", &SGA::Tile::x)
			.def("y", &SGA::Tile::y)
			;

		// ---- Grid2D<Tile> ----
		py::class_<SGA::Grid2D<SGA::Tile>>(m, "BoardTiles", "Contains a 2D grid of tiles.")
			.def(py::init<int, int, SGA::Tile>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
			//.def_readwrite("grid", &SGA::Grid2D<SGA::Tile>::grid)
			.def("get", [](SGA::Grid2D<SGA::Tile>& b, int x, int y) {return b.get(x, y); })
			.def("get_width", &SGA::Grid2D<SGA::Tile>::getWidth)
			.def("get_height", &SGA::Grid2D<SGA::Tile>::getHeight)
			.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
			.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
			;
		// ---- Grid2D<TileType> ----
		py::class_<SGA::Grid2D<std::shared_ptr<SGA::TileType>>>(m, "BoardTileTypes", "Contains a 2D grid of tile types.")
			.def(py::init<int, int, std::shared_ptr<SGA::TileType>>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
			//.def_readwrite("grid", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::grid)
			.def("get", [](const SGA::Grid2D<std::shared_ptr<SGA::TileType>>& b, int x, int y) {return b.get(x, y); })
			.def("get_width", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getWidth)
			.def("get_height", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getHeight)
			.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
			.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
			;
	}	
}