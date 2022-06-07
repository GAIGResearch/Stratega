#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>

#include <Stratega/Logging/FileLogger.h>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Arena/Arena.h>
#include <Stratega/Representation/Buff.h>
#include <Stratega/Representation/BuffType.h>
#include <fstream>
#include <sstream>

#include <Stratega/Logging/Log.h>
#include <limits>
#include <Stratega/Utils/Timer.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max

namespace py = pybind11;

namespace stratega
{
	void game(py::module_& m)
	{
		// ---- GameType ----
		py::enum_<SGA::GameType>(m, "GameType")
			.value("TBS", SGA::GameType::TBS)
			.value("RTS", SGA::GameType::RTS)
			.export_values();

		

		// ---- GameConfig ----
		py::class_<SGA::GameConfig>(m, "GameConfig")
			.def_readwrite("game_type", &SGA::GameConfig::gameType)

			.def_readwrite("tick_limit", &SGA::GameConfig::tickLimit)
			.def_readwrite("num_players", &SGA::GameConfig::numPlayers)
			.def_readwrite("tile_types", &SGA::GameConfig::tileTypes)

			.def_property_readonly("forward_model", [](const SGA::GameConfig& config) {return config.forwardModel.get(); }, py::return_value_policy::reference_internal)

			//Players
			.def_readwrite("player_parameter_types", &SGA::GameConfig::playerParameterTypes)
			.def_readwrite("player_spawnable_types", &SGA::GameConfig::playerSpawnableTypes)
			.def_readwrite("player_action_ids", &SGA::GameConfig::playerActionIds)

			//Entities
			.def_readwrite("parameters", &SGA::GameConfig::parameters)
			.def_readwrite("entity_groups", &SGA::GameConfig::entityGroups)
			.def_readwrite("entity_types", &SGA::GameConfig::entityTypes)

			//Actions types		
			.def_readwrite("action_types", &SGA::GameConfig::actionTypes)

			//Levels
			.def_readwrite("default_tile_type_id", &SGA::GameConfig::defaultTileTypeID)
			.def_readwrite("level_definitions", &SGA::GameConfig::levelDefinitions)
			.def_readwrite("selected_level", &SGA::GameConfig::selectedLevel)

			// Technology tree
			.def_readwrite("technology_tree_collection", &SGA::GameConfig::technologyTreeCollection)

			//ActionCategories and EntityCategories
			.def_readwrite("action_categories", &SGA::GameConfig::actionCategories)
			.def_readwrite("entity_categories", &SGA::GameConfig::entityCategories)

			.def_readwrite("yaml_path", &SGA::GameConfig::yamlPath)
			.def_readwrite("resources_path", &SGA::GameConfig::resourcesPath)

			.def("generate_agents", &SGA::GameConfig::generateAgents)
			.def("generate_gamestate", &SGA::GameConfig::generateGameState, py::arg("levelID") = -1)
			.def("get_forward_model",
				[](SGA::GameConfig* a)
		{
			return std::move(a->forwardModel);
		}
			)
			//Methods
			.def("get_number_of_players", &SGA::GameConfig::getNumberOfPlayers)
			.def("get_entity_id", &SGA::GameConfig::getEntityID, py::arg("name"))
			.def("get_tile_id", &SGA::GameConfig::getTileID, py::arg("name"))
			.def("get_action_id", &SGA::GameConfig::getActionID, py::arg("name"))
			.def("get_technology_id", &SGA::GameConfig::getTechnologyID, py::arg("name"))
			;
		// ---- GameInfo ----
		py::class_<SGA::GameInfo, std::shared_ptr<SGA::GameInfo>>(m, "GameInfo")

			.def("get_entity_type", &SGA::GameInfo::getEntityType, py::arg("entityTypeID"), "Returns the entity type.")
			.def("get_tile_type", &SGA::GameInfo::getTileType, py::arg("tileTypeID"), "Returns the tile type.")

			.def("get_parameter_global_id", &SGA::GameInfo::getParameterGlobalID, py::arg("parameterName"), "Returns the ID of the parameter.")
			.def("get_parameter_type", &SGA::GameInfo::getParameterType, py::arg("entityTypeID"), py::arg("globalParameterID"), "Returns the parameter type from a entityType.")
			.def("check_entity_have_parameter", &SGA::GameInfo::checkEntityHaveParameter, py::arg("entityTypeID"), py::arg("parameterName"), "Returns true if the entity type has the searched parameter")
			//.def("get_player_parameter", &SGA::GameInfo::getPlayerParameter, py::arg("id"), "Returns the parameter of the player.")

			.def("get_action_type_const", &SGA::GameInfo::getActionTypeConst, py::arg("actionTypeID"), "Returns the action type.")
			.def("get_action_type", &SGA::GameInfo::getActionType, py::arg("typeID"), "Returns the action type.")
			.def("get_action_type_id", &SGA::GameInfo::getActionTypeID, py::arg("actionName"), "Returns the action type ID.")

			.def("get_buff_type", &SGA::GameInfo::getBuffType, py::arg("buffTypeID"), "Returns the buff type by ID.")

			.def("get_technology_counts", &SGA::GameInfo::getTechnologyCounts, "Returns a map of tuples with the tree ID and how many technologies each has.")
			.def("get_technology", &SGA::GameInfo::getTechnology, py::arg("technologyID"), "Returns the action type.")
			.def("get_technology_type_id", &SGA::GameInfo::getTechnologyTypeID, py::arg("technologyName"), "Returns the ID of a technology that matches the requested name, in any of the trees of the tree collection.")
			.def("get_tree_nodes", &SGA::GameInfo::getTreeNodes, py::arg("techTreeID"), "Returns a list with all technologies of a given tree, specified by its ID")
			.def("get_tech_trees_ids", &SGA::GameInfo::getTechTreesIDs, "Returns the IDs of the tech trees of this game.")

			.def("get_game_description", &SGA::GameInfo::getGameDescription, "Returns the game description of the game.")
			.def("get_yaml_path", &SGA::GameInfo::getYAMLPath)
			.def("get_parameter_id_lookup", &SGA::GameInfo::getParameterIDLookup)
			.def("get_player_parameter_types", &SGA::GameInfo::getPlayerParameterTypes)
			.def("get_player_spawnable_types", &SGA::GameInfo::getPlayerSpawnableTypes)
			.def("get_buff_types", &SGA::GameInfo::getBuffTypes)
			.def("get_entity_types", &SGA::GameInfo::getEntityTypes)
			.def("get_action_types", &SGA::GameInfo::getActionTypes)
			.def("get_tile_types", &SGA::GameInfo::getTileTypes)
			.def("get_technology_tree_collection", &SGA::GameInfo::getTechnologyTreeCollection)
			.def("get_entity_groups", &SGA::GameInfo::getEntityGroups)

			.def("set_yaml_path", &SGA::GameInfo::setYAMLPath, py::arg("path"))
			.def("set_technology_tree_collection", &SGA::GameInfo::setTechnologyTreeCollection, py::arg("tt"))
			.def("set_entity_groups", &SGA::GameInfo::setEntityGroups, py::arg("tg"))
			;

		// ---- EntityPlacement ----	
		py::class_<SGA::EntityPlacement>(m, "EntityPlacement")
			.def(py::init<>())
			.def_readwrite("ownerID", &SGA::EntityPlacement::ownerID)
			.def_readwrite("position", &SGA::EntityPlacement::position)
			.def_readwrite("entity_type", &SGA::EntityPlacement::entityType)
			;

		// ---- LevelDefinition ----	
		py::class_<SGA::LevelDefinition>(m, "LevelDefinition")
			.def(py::init<std::vector<SGA::EntityPlacement>, SGA::Grid2D<std::shared_ptr<SGA::TileType>>>(), py::arg("entityPlacements"), py::arg("board"))
			.def_readwrite("name", &SGA::LevelDefinition::name)
			.def_readwrite("board_string", &SGA::LevelDefinition::boardString)
			.def_readwrite("entity_placements", &SGA::LevelDefinition::entityPlacements)
			.def_readwrite("board", &SGA::LevelDefinition::board)
			;
	}	
}