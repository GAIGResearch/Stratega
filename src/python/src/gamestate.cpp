#include "headers.h"

namespace stratega
{
	void gamestate(py::module_& m)
	{
		// ---- GameState ----
		py::class_<SGA::GameState>(m, "GameState", "Contains the game data without any logic, offering access to the current board, a list of player and their units.  If the agent want access to the definition of entity types, actions or game config yaml  it should access to SGA::GameInfo")
			.def(py::init<SGA::GameState>())
			.def(py::init([]()
		{
			py::scoped_ostream_redirect stream(
				std::cout,                               // std::ostream&
				py::module_::import("sys").attr("stdout") // Python output
			);

			std::cerr << "A GameState was constructed, use XXX instead" << std::endl;
			return std::make_unique<SGA::GameState>();

		}))
			.def("__copy__", [](const SGA::GameState& self) {
			return SGA::GameState(self);
		})
			.def("__deepcopy__", [](const SGA::GameState& self, py::dict) {
			return SGA::GameState(self);
		})


			//Other
			.def("can_execute_action", py::overload_cast<const SGA::Entity&, const SGA::ActionType&>(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Entity can execute a given actionType")
			.def("can_execute_action", py::overload_cast<const SGA::Player&, const SGA::ActionType&>(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Player can execute a given actionType")
			.def("get_player_action_types", &SGA::GameState::getPlayerActionTypes, py::arg("playerID"), "Returns a list with all the action types that a player can execute")

			//Technologies
			.def("is_researched", &SGA::GameState::isResearched, py::arg("playerID"), py::arg("technologyID"), "Check if technology is researched")
			.def("can_research", &SGA::GameState::canResearch, py::arg("playerID"), py::arg("technologyID"), "Check if player can research an technology")
			.def("research_technology", &SGA::GameState::researchTechnology, py::arg("playerID"), py::arg("technologyID"), "Research technology")
			.def("init_research_techs", &SGA::GameState::initResearchTechs, "Initializes the research technologies to all players, to none.")

			////Grid
			.def("is_walkable", &SGA::GameState::isWalkable, py::arg("position"), "Checks if tile is occupied or the tile is walkable")
			.def("is_in_bounds", py::overload_cast<const SGA::Vector2f&>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")
			.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")

			.def("initBoard", &SGA::GameState::initBoard, py::arg("boardWidth"), py::arg("tiles"), "Initializes the board with the tiles passed by parameter. ")

			.def("get_board_height", &SGA::GameState::getBoardHeight, "Returns the height of the board.")
			.def("get_board_width", &SGA::GameState::getBoardWidth, "Returns the width of the board.")

			.def("get_tile_at", py::overload_cast<int, int>(&SGA::GameState::getTileAtConst, py::const_), "Returns the tile at the position indicated in the parameter. Can throw an exception if out of bounds.")
			.def("get_tile_at", py::overload_cast<const SGA::Vector2i&>(&SGA::GameState::getTileAtConst, py::const_), "Returns the tile at the position (x,y) indicated in the parameter. Can throw an exception if out of bounds.")

			////Entities
			.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity")
			.def("get_entity", py::overload_cast<SGA::Vector2f, float>(&SGA::GameState::getEntityAround), py::return_value_policy::reference, "Get entity")
			.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity by id")
			.def("get_entity_at", &SGA::GameState::getEntityAtConst, py::arg("pos"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
			.def("get_only_entities", &SGA::GameState::getOnlyEntities, py::arg("entityID"), py::return_value_policy::reference, "Returns an entity by its ID. It'll return nullptr if no entity exists associated to the given ID. It only search the entities in the gamestate without the objects of the inventories. ")
			.def("get_object", &SGA::GameState::getObject, py::arg("entityID"), py::return_value_policy::reference, "Return object by its ID")
			.def("get_slot_object", &SGA::GameState::getSlotObject, py::arg("entityID"), py::return_value_policy::reference, "Return slot object by its ID")
			.def("get_entity_const", &SGA::GameState::getEntityConst, py::arg("entityID"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
			.def("add_entity", py::overload_cast<const SGA::EntityType&, int, const SGA::Vector2f&>(&SGA::GameState::addEntity), "Adds a new entity of a given type to the game, in a given position, belonging to a specific player. ")
			.def("add_entity", py::overload_cast<SGA::Entity, int, const SGA::Vector2f&>(&SGA::GameState::addEntity), "Adds a new entity, in a given position, belonging to a specific player. ")

			.def("get_player_entities", &SGA::GameState::getPlayerEntities, py::arg("playerID"), py::arg("entityCategory") = SGA::EntityCategory::Null, "Gets the list of entities of the specified player.")
			.def("get_non_player_entities", &SGA::GameState::getNonPlayerEntities, py::arg("playerID"), py::arg("entityCategory") = SGA::EntityCategory::Null, "Gets the list of entities of the specified player.")
			.def("get_entities",
				[](SGA::GameState& state)
		{
			return state.getEntities();
		}
			)
			////Player
			.def("get_player",
				[](SGA::GameState& state, int id)
		{
			return state.getPlayer(id);
		}
				, py::return_value_policy::reference, "Get player"
			)
			.def("who_can_play", &SGA::GameState::whoCanPlay, "Returns a list with the ID of players that can play in this game state.")
			.def("can_play", &SGA::GameState::canPlay, py::arg("playerID"), "Indicates if the player with the provided ID can play in this game state.")
			.def("get_players",
				[](SGA::GameState& state)
		{
			return state.getPlayers();
		}
				, py::return_value_policy::reference, "Get player"
			)
			.def("get_num_players", &SGA::GameState::getNumPlayers, "Gets the number of players in this game state.")
			.def("add_player", &SGA::GameState::addPlayer, py::arg("p"), "Adds player to gamestate")
			.def("get_player_parameter", &SGA::GameState::getPlayerParameter, py::arg("playerID"), py::arg("paramName"), "Gets the value of a player parameter. Be sure to check first is the parameter you are asking for exist using SGA::GameState::hasPlayerParameter()")
			.def("has_player_parameter", &SGA::GameState::hasPlayerParameter, py::arg("paramName"), "Indicates if the player has a specific parameter")
			.def("get_player_parameter_names", &SGA::GameState::hasPlayerParameter, py::arg("playerID"), "Returns a list will all the parameter names of the player of which ID is given")
			.def("get_player_parameters", &SGA::GameState::getPlayerParameters, py::arg("playerID"), "Gets a map with all pairs <parameter,value>")
			.def("get_player_score", &SGA::GameState::getPlayerScore, py::arg("playerID"), "Returns the score of the player whose ID is passed.")

			.def("apply_fog_of_war", &SGA::GameState::applyFogOfWar, py::arg("playerID"), "Removes entities and hide tiles that are not visible from the point of view of the given player.")
			.def("get_fog_of_war_tile_id", &SGA::GameState::getFogOfWarTileId, "Returns the ID of the tile that represents the fog of war.")

			.def("get_current_tbs_player", &SGA::GameState::getCurrentTBSPlayer, "Returns the ID of the player that moves in this state for Turn Based Games. For non-TBS, this value is -1.")
			.def("set_current_tbs_player", &SGA::GameState::setCurrentTBSPlayer, py::arg("playerID"), "Sets the current TBS player. For non-TBS, this should receive -1.")

			.def_property("game_over", &SGA::GameState::isGameOver, &SGA::GameState::setGameOver)
			.def("is_game_over", &SGA::GameState::isGameOver, "Returns true if the game is over.")
			.def("set_game_over", &SGA::GameState::setGameOver, py::arg("over"), "Sets if the game is over")

			.def_property("winner_id", &SGA::GameState::getWinnerID, &SGA::GameState::setWinnerID)
			.def("get_winner_id", &SGA::GameState::getWinnerID, "Returns the player ID of the winner. If game is not over, this returns -1.")
			.def("set_winner_id", &SGA::GameState::setWinnerID, py::arg("winnerID"), "Sets the winner of the game.")

			.def_property("current_tick", &SGA::GameState::getCurrentTick, &SGA::GameState::setTickLimit)
			.def("get_current_tick", &SGA::GameState::getCurrentTick, "Returns the current tick of the game.")
			.def("inc_tick", &SGA::GameState::incTick, "Increments the current tick in the game by 1.")
			.def("set_tick_limit", &SGA::GameState::setTickLimit, py::arg("tickL"), "Returns the current game tick limit.")

			.def_property("game_info", &SGA::GameState::getGameInfo, &SGA::GameState::setGameInfo)
			.def("get_game_info", &SGA::GameState::getGameInfo, "Returns a pointer to the struct with static information about the game.")
			.def("set_game_info", &SGA::GameState::setGameInfo, py::arg("gameInfoPtr"), "Sets the pointer to the game information struct.")

			.def_property("game_type", &SGA::GameState::setGameType, &SGA::GameState::getGameType)
			.def("set_game_type", &SGA::GameState::setGameType, py::arg("gt"), "Sets the type of game (enum type GameType)")
			.def("get_game_type", &SGA::GameState::getGameType, "Returns the type of the game, of GameType")

			//Print information
			.def("print_state_info", &SGA::GameState::printStateInfo, "Print all the entities of the current state")
			.def("print_board", py::overload_cast<int>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state")
			.def("print_board", py::overload_cast<>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state applying fog")
			.def("print_entity_info", &SGA::GameState::printEntityInfo, py::arg("entityID"), "Print information of a specific entity")
			.def("print_action_info", &SGA::GameState::printActionInfo, py::arg("action"), "Print information of a specific action")
			;
	}
}