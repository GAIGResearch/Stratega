#include "headers.h"

namespace stratega
{
	void forwardmodel(py::module_& m)
	{
		//---- GameDescription ----
		py::class_<SGA::GameDescription, std::shared_ptr<SGA::GameDescription>>(m, "GameDescription", "Represents a grouping of action and entity types into categories")
			.def("get_action_categories", &SGA::GameDescription::getActionCategories, "Returns the action categories of this game.")
			.def("get_entity_categories", &SGA::GameDescription::getEntityCategories, "Returns the entity categories of this game.")
			.def("get_action_types_ids", &SGA::GameDescription::getActionTypesIDs, py::arg("category"), "Returns all the actiontypes IDs of the selected action category.")
			.def("get_entity_types_ids", &SGA::GameDescription::getEntityTypesIDs, py::arg("category"), "Returns all the entitytypes IDs of the selected action category.")
			.def("get_action_types", &SGA::GameDescription::getActionTypes, py::arg("category"), py::arg("gameInfo"), "Returns all the actiontypes IDs of the selected action category.")
			.def("get_entity_types", &SGA::GameDescription::getEntityTypes, py::arg("category"), py::arg("gameInfo"), "Returns all the entitytypes IDs of the selected action category.")
			.def("is_from_category", &SGA::GameDescription::isFromCategory, py::arg("category"), py::arg("entityTypeId"), "Checks if a given entity type ID belongs to a given category")

			//.def_static("to_string", py::overload_cast<SGA::ActionCategory>(&SGA::GameDescription::toString))
			//.def_static("to_string", py::overload_cast<SGA::EntityCategory>(&SGA::GameDescription::toString))
			;

		// ---- EntityCategory ----
		py::enum_<SGA::EntityCategory>(m, "EntityCategory", "Represents a grouping of entity types into categories. Can be used by the agent to query SGA::EntityType grouped in different categories.")
			.value("Null", SGA::EntityCategory::Null)
			.value("Base", SGA::EntityCategory::Base)
			.value("Building", SGA::EntityCategory::Building)
			.value("Spawner", SGA::EntityCategory::Spawner)
			.value("Unit", SGA::EntityCategory::Unit)
			.value("NoFighter", SGA::EntityCategory::NoFighter)
			.value("Fighter", SGA::EntityCategory::Fighter)
			.value("Melee", SGA::EntityCategory::Melee)
			.value("Ranged", SGA::EntityCategory::Ranged)
			.export_values()
			;

		// ---- ActionCategory ----
		py::enum_<SGA::ActionCategory>(m, "ActionCategory", "Represents a grouping of action types into categories. Can be used by the agent to query SGA::ActionType grouped in different categories.")
			.value("Null", SGA::ActionCategory::Null)
			.value("Move", SGA::ActionCategory::Move)
			.value("Attack", SGA::ActionCategory::Attack)
			.value("Heal", SGA::ActionCategory::Heal)
			.value("Gather", SGA::ActionCategory::Gather)
			.value("Spawn", SGA::ActionCategory::Spawn)
			.value("Research", SGA::ActionCategory::Research)
			.export_values()
			;

		// ---- OnTickEffect ----
		py::class_<SGA::OnTickEffect>(m, "OnTickEffect", "Set of effects executed every tick")
			.def(py::init<>())
			.def_readwrite("valid_targets", &SGA::OnTickEffect::validTargets)
			.def_readwrite("conditions", &SGA::OnTickEffect::conditions)
			.def_readwrite("effects", &SGA::OnTickEffect::effects)
			;
		// ---- OnEntitySpawnEffect ----
		py::class_<SGA::OnEntitySpawnEffect>(m, "OnEntitySpawnEffect", "Set of effects executed every entity is spawned")
			.def(py::init<>())
			.def_readwrite("valid_targets", &SGA::OnEntitySpawnEffect::validTargets)
			.def_readwrite("conditions", &SGA::OnEntitySpawnEffect::conditions)
			.def_readwrite("effects", &SGA::OnEntitySpawnEffect::effects)
			;

		// ---- Forward model ----
		py::class_<SGA::ForwardModel>(m, "EntityForwardModel", "Is the driving component of Stratega, with a provided gamestate it can generate a new set of available actions thanks to the")
			.def("can_player_lost", &SGA::ForwardModel::checkPlayerLost, py::arg("state"), py::arg("player"), "Checks if a player has lost the game due to the game's lose conditions.")
			.def("check_player_won", &SGA::ForwardModel::checkPlayerWon, py::arg("state"), py::arg("player"), "Returns if a player won the game by attending to the winning conditions defined in the rules.")
			.def("spawn_entity", &SGA::ForwardModel::spawnEntity, py::arg("state"), py::arg("entityType"), py::arg("playerID"), py::arg("position"), "Spawns an entity in the game with the default spawn entity method. A list of On Spawn effects are  executed just after spawning the entity")

			.def("add_win_conditions", &SGA::ForwardModel::addWinConditions, py::arg("conditions"), " Adds a list of conditions for the game to be won for a player.")
			.def("add_lose_conditions", &SGA::ForwardModel::addLoseConditions, py::arg("conditions"), "Adds a list of conditions for the game to be lost for a player.")
			.def("get_win_conditions", &SGA::ForwardModel::getWinConditions, "Returns a list of sub-lists with all WIN conditions. Each sub-list contains a group of conditions that must be fulfilled for the game to be over for a player. The game will be WON by a player if all conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. ")
			.def("get_lose_conditions", &SGA::ForwardModel::getLoseConditions, "Returns a list of sub-lists with all LOSE conditions. Each sub-list contains a group of conditions  that must be fulfilled for the game to be over for a player. The game will be LOST by a player if all  conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. ")

			.def("add_on_tick_effect", &SGA::ForwardModel::addOnTickEffect, py::arg("ote"), "Adds an OnTickEffect to the forward mode, which will be executed every game tick.")
			.def("add_on_entity_spawn_effect", &SGA::ForwardModel::addOnEntitySpawnEffect, py::arg("ose"), "Adds an OnEntitySpawnEffect to the forward mode, which will be executed every time an entity is spawned.")

			.def("get_on_tick_effects", &SGA::ForwardModel::getOnTickEffects, "Returns all effects that are exxecuted on every tick of the game.")
			.def("get_on_entity_spawn_effects", &SGA::ForwardModel::getOnEntitySpawnEffects, "Returns all effects that are exxecuted every time an entity is spawned in the game.")
			;


		py::class_<SGA::TBSForwardModel, SGA::ForwardModel>(m, "TBSForwardModel", "Is the default SGA::ForwardModel for TBS games, it contains specific methods.")
			.def(py::init<>())

			.def("get_game_type", &SGA::TBSForwardModel::getGameType)

			.def("generate_actions", py::overload_cast<const SGA::GameState&, int>(&SGA::ForwardModel::generateActions, py::const_), " Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.")
			.def("generate_actions", py::overload_cast<const SGA::GameState&, int, std::vector<SGA::Action>&>(&SGA::ForwardModel::generateActions, py::const_), "Returns a list of available actions in the given gamestate by the received player")

			.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::Action&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes an action in a given SGA::GameState before updating the entities of the gamestate that should be removed and checking if the game is over.")
			.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::ActionAssignment&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes a list of actions.")

			.def("end_turn", &SGA::TBSForwardModel::endTurn, py::arg("state"), "End the turn of the current player and if all the player has played it ends the current game turn.")
			.def("check_game_is_finished", &SGA::TBSForwardModel::checkGameIsFinished, py::arg("state"), " Checks if the game is finished by current limit or because a player has won.")
			;

		py::class_<SGA::RTSForwardModel, SGA::ForwardModel>(m, "RTSForwardModel", "Is the default SGA::ForwardModel for RTS games, it contains specific methods.")
			.def(py::init<>())
			.def("get_game_type", &SGA::RTSForwardModel::getGameType)

			.def("generate_actions", py::overload_cast<const SGA::GameState&, int>(&SGA::ForwardModel::generateActions, py::const_), " Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.")
			.def("generate_actions", py::overload_cast<const SGA::GameState&, int, std::vector<SGA::Action>&>(&SGA::ForwardModel::generateActions, py::const_), "Returns a list of available actions in the given gamestate by the received player")

			.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::Action&>(&SGA::RTSForwardModel::advanceGameState, py::const_), "Moves all the entities and resolves collisions before and after executing an action in a given Gamestate")
			.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::ActionAssignment&>(&SGA::RTSForwardModel::advanceGameState, py::const_), "Moves all the entities and resolves collisions before and after executing an action in a given Gamestate")

			.def("move_entities", &SGA::RTSForwardModel::moveEntities, py::arg("state"), "Moves all the entities that have a current path and they did not reach their destination. If the entity has a path it moves the entity through all the path points one after theother until reaching the last one.")
			.def("resolve_entity_collisions", &SGA::RTSForwardModel::resolveEntityCollisions, py::arg("state"), "Resolves collisions between entities in a basic way computing the penetration depth and pushing them way in the opposite direction.")
			.def("resolve_environment_collisions", &SGA::RTSForwardModel::resolveEnvironmentCollisions, py::arg("state"), "Resolves collisions between entities and the tiles that are not walkable in a basic way computing the penetration depth and pushing them way in the opposite direction.")

			.def("find_path", &SGA::RTSForwardModel::findPath, py::arg("state"), py::arg("startPos"), py::arg("endPos"), "Returns a Path inside the Navmesh between the start and end positons.")
			.def("check_game_is_finished", &SGA::RTSForwardModel::checkGameIsFinished, py::arg("state"), "Checks if the game is finished by current limit or because a player has won.")
			;

		// ---- Path ----
		py::class_<SGA::Path>(m, "Path", "Contains all the points of the path and the current point index. Used by the default SGA::RTSForwardModel for moving entities.")
			.def(py::init<>())
			//.def_readwrite("m_straight_path", &SGA::Path::m_straightPath)
			.def_readwrite("m_nstraight_path", &SGA::Path::m_nstraightPath)
			//.def_readwrite("m_straight_path_flags", &SGA::Path::m_straightPathFlags)
			.def_readwrite("m_straight_path_options", &SGA::Path::m_straightPathOptions)
			.def_readwrite("current_path_index", &SGA::Path::currentPathIndex)
			.def("is_empty", &SGA::Path::isEmpty);
	}	
}