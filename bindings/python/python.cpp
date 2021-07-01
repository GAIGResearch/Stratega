#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>

#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace py = pybind11;
class PythonAgent;

// STL
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Action>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Player>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Agent*>);
PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SGA::Agent>>);
PYBIND11_MAKE_OPAQUE(std::vector<PythonAgent>);

class PyCondition : public SGA::Condition {
public:
	/* Inherit the constructors */
	using SGA::Condition::Condition;

	/* Trampoline (need one for each virtual function) */
	bool isFullfilled(const SGA::GameState& state, const std::vector<SGA::ActionTarget>& targets)const override {
		PYBIND11_OVERRIDE_PURE(
			bool, /* Return type */
			SGA::Condition,      /* Parent class */
			isFullfilled,          /* Name of function in C++ (must match Python name) */
			state,
			targets/* Argument(s) */
		);
	}
};

class PySamplingMethod : public SGA::SamplingMethod {
public:
	/* Inherit the constructors */
	using SGA::SamplingMethod::SamplingMethod;

	/* Trampoline (need one for each virtual function) */
	std::vector<SGA::Vector2i>getPositions(const SGA::GameState& gameState, const SGA::Vector2f& position)const override
	{
		PYBIND11_OVERRIDE_PURE(
			std::vector<SGA::Vector2i>, /* Return type */
			SGA::SamplingMethod,      /* Parent class */
			getPositions,          /* Name of function in C++ (must match Python name) */
			gameState,
			position/* Argument(s) */
		);
	}

	/* Trampoline (need one for each virtual function) */
	std::vector<int>getEntities(const SGA::GameState& gameState, const SGA::Vector2f& position, const std::unordered_set<int>& entityTypeIDs)const override
	{
		PYBIND11_OVERRIDE_PURE(
			std::vector<int>, /* Return type */
			SGA::SamplingMethod,      /* Parent class */
			getEntities,          /* Name of function in C++ (must match Python name) */
			gameState,
			position,/* Argument(s) */
			entityTypeIDs/* Argument(s) */
		);
	}
};

class PyEffect : public SGA::Effect {
public:
	/* Inherit the constructors */
	using SGA::Effect::Effect;

	/* Trampoline (need one for each virtual function) */
	void execute(SGA::GameState& state, const SGA::EntityForwardModel& fm, const std::vector<SGA::ActionTarget>& targets)const override {
		PYBIND11_OVERRIDE_PURE(
			void, /* Return type */
			SGA::Effect,      /* Parent class */
			execute,          /* Name of function in C++ (must match Python name) */
			state,
			fm,
			targets/* Argument(s) */
		);
	}
};

class PyAgent : public SGA::Agent {
public:
	/* Inherit the constructors */
	using SGA::Agent::Agent;
	using SGA::Agent::getPlayerID;

	/* Trampoline (need one for each virtual function) */
	SGA::ActionAssignment computeAction(SGA::GameState state, const SGA::EntityForwardModel* forwardModel, long timeBudgetMs) override
	{
		PYBIND11_OVERRIDE_PURE(
			SGA::ActionAssignment,
			SGA::Agent,
			computeAction,
			state,
			forwardModel,
			timeBudgetMs
		);
	}

	void computeAction2(SGA::GameState state, const SGA::EntityForwardModel* forwardModel, long timeBudgetMs) override
	{
		PYBIND11_OVERRIDE(
			void,
			SGA::Agent,
			computeAction2,
			state,
			forwardModel,
			timeBudgetMs
		);
	}
};

 std::unique_ptr<SGA::GameConfig> loadConfig(std::string& path)
 {
	 return SGA::loadConfigFromYAML(path);
 }

 std::unique_ptr<SGA::GameRunner> createRunner(const SGA::GameConfig* gameConfig)
 {
	 return SGA::createGameRunner(*gameConfig);
 }

 std::vector<std::shared_ptr<SGA::Agent>> generateAgents(const SGA::GameConfig* gameConfig)
 {
	 return gameConfig->generateAgents();
 }

 void initializeAgents(std::vector<std::shared_ptr<SGA::Agent>>& agents, int seed)
 {
	 std::mt19937 rngEngine(seed);
	 // Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
	 std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
	 for (auto& agent : agents)
	 {
		 auto seed = seedDist(rngEngine);
		 // Ignore human player
		 if (agent != nullptr)
		 {
			 agent->setSeed(seed);
		 }
	 }
 }

PYBIND11_MODULE(stratega, m)
{
    m.doc() = "Stratega python bindings"; // optional module docstring

	m.def("load_config", &loadConfig, "Loads game config", py::arg("path"));
	m.def("create_runner", &createRunner, "Create game runner", py::arg("gameConfig"));
	m.def("generate_agents", &generateAgents, "Generate agents", py::arg("gameConfig"));
	m.def("initialize_agents", &initializeAgents, "Initialize agents", py::arg("agents"), py::arg("seed"));
 	
	// ---- STL binds----
	//Basic list
	py::bind_vector<std::vector<double>>(m, "DoubleList");
	py::bind_vector<std::vector<int>>(m, "IntList");

	py::bind_vector<std::vector<SGA::Player>>(m, "PlayerList");
	py::bind_vector<std::vector<SGA::Entity>>(m, "EntityList");

	py::bind_vector<std::vector<std::shared_ptr<SGA::Agent>>>(m, "AgentList");

	py::bind_vector<std::vector<SGA::ActionTarget>>(m, "ActionTargetList");

	py::bind_vector<std::vector<std::shared_ptr<SGA::EntityType>>>(m, "EntityTypetList");
	py::bind_vector<std::vector<SGA::EntityPlacement>>(m, "EntityPlacementList");

	//Technologies
	py::bind_map<std::unordered_map<int, SGA::TechnologyTreeNode>>(m, "TechnologiesMap");
	py::bind_map<std::unordered_map<int, std::vector<int>>>(m, "ResearchedTechnologiesMap");

	//Types
	py::bind_map<std::unordered_map<int, SGA::TileType>>(m, "TileTypesMap");
	py::bind_map<std::unordered_map<int, SGA::EntityType>>(m, "EntityTypesMap");
	py::bind_map<std::unordered_map<int, SGA::ActionType>>(m, "ActionTypesMap");

	//Actions stuff:
	py::bind_vector<std::vector<std::pair<SGA::TargetType, std::vector<std::shared_ptr<SGA::Condition>>>>>(m, "ActionTargetsList");
	py::bind_vector<std::vector<std::shared_ptr<SGA::Condition>>>(m, "ConditionsList");
	py::bind_vector<std::vector<std::shared_ptr<PyCondition>>>(m, "PyConditionsList");
	py::bind_vector<std::vector<std::shared_ptr<SGA::Effect>>>(m, "EffectsList");

	//Parameters
	py::bind_map<std::unordered_map<std::string, SGA::ParameterID>>(m, "ParametersMap");
	py::bind_map<std::unordered_map<SGA::ParameterID, SGA::Parameter>>(m, "PlayerParameterTypesMap");

	//Entities
	py::bind_map<std::unordered_map<std::string, std::unordered_set<int>>>(m, "EntityGroupsMap");

	//Actions
	py::bind_map<std::unordered_map<int, SGA::Action>>(m, "ActionsMap");

	//Levels
	py::bind_map<std::unordered_map<int, SGA::LevelDefinition>>(m, "LevelDefinitionsMap");

	//Cost
	py::bind_map<std::unordered_map<SGA::ParameterID, double>>(m, "CostMap");

	//Categories
	py::bind_map<std::unordered_map<SGA::ActionCategory, std::vector<int>>>(m, "ActionCategoriesMap");
	py::bind_map<std::unordered_map<SGA::EntityCategory, std::vector<int>>>(m, "EntityCategoriesMap");

	
	
	py::class_<std::vector<SGA::Action>>(m, "ActionList")
		.def(py::init<>())
		.def("__copy__", [](const std::vector<SGA::Action>& self) {
		return std::vector<SGA::Action>(self);
			})
		.def("__deepcopy__", [](const std::vector<SGA::Action>& self, py::dict) {
				return std::vector<SGA::Action>(self);
			})
		.def("clear", &std::vector<SGA::Action>::clear)
		.def("get", [](const std::vector<SGA::Action>& v, int index)
			{
				return v[index]; 
			})
		.def("pop_back", &std::vector<SGA::Action>::pop_back)
		.def("count", [](const std::vector<SGA::Action>& v) { return v.size(); })
		.def("__len__", [](const std::vector<SGA::Action>& v) { return v.size(); })
		.def("__iter__", [](std::vector<SGA::Action>& v) {
				return py::make_iterator(v.begin(), v.end());
					}, py::keep_alive<0, 1>());
			

	// ---- Vector2f ----
	py::class_<SGA::Vector2f>(m, "Vector2f")
		.def("__copy__", [](const SGA::Vector2f& self) {
		return SGA::Vector2f(self);
			})
		.def("__deepcopy__", [](const SGA::Vector2f& self, py::dict) {
				return SGA::Vector2f(self);
			})
		.def(py::init<double, double>(), py::arg("x") = 0.0, py::arg("y") = 0.0)
		.def_readwrite("x", &SGA::Vector2f::x)
		.def_readwrite("y", &SGA::Vector2f::y)
		.def("__repr__", [](const SGA::Vector2f& v) {return "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")"; });

	// ---- Vector2i ----
	py::class_<SGA::Vector2i>(m, "Vector2i")
		.def("__copy__", [](const SGA::Vector2i& self) {
		return SGA::Vector2i(self);
			})
		.def("__deepcopy__", [](const SGA::Vector2i& self, py::dict) {
				return SGA::Vector2i(self);
			})
		.def(py::init<int, int>(), py::arg("x") = 0.0, py::arg("y") = 0.0)
		.def_readwrite("x", &SGA::Vector2i::x)
		.def_readwrite("y", &SGA::Vector2i::y)
		.def("__repr__", [](const SGA::Vector2i& v) {return "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")"; });
	
	// ---- Action type ----
	py::class_<SGA::ActionType, std::shared_ptr<SGA::ActionType>>(m, "ActionType")
		.def(py::init<>())
		.def_readwrite("name", &SGA::ActionType::name)
		.def_readwrite("source_type", &SGA::ActionType::sourceType)
		.def_readwrite("id", &SGA::ActionType::id)
		.def_readwrite("cooldown_ticks", &SGA::ActionType::cooldownTicks)
		.def_readwrite("action_targets", &SGA::ActionType::actionTargets)
		.def_readwrite("preconditions", &SGA::ActionType::preconditions)
		.def_readwrite("effects", &SGA::ActionType::effects)
		.def_readwrite("is_continuous", &SGA::ActionType::isContinuous)
		.def_readwrite("trigger_complete", &SGA::ActionType::triggerComplete)
		.def_readwrite("on_start", &SGA::ActionType::OnStart)
		.def_readwrite("on_tick", &SGA::ActionType::OnTick)
		.def_readwrite("on_complete", &SGA::ActionType::OnComplete)
		.def_readwrite("on_abort", &SGA::ActionType::OnAbort)
		.def("get_target_conditions", &SGA::ActionType::getTargetConditions, py::arg("searchingTarget"));

	// ---- Entity type ----
	py::class_<SGA::EntityType, std::shared_ptr<SGA::EntityType>>(m, "EntityType")
		.def(py::init<>())
		.def_readwrite("name", &SGA::EntityType::name)
		.def_readwrite("id", &SGA::EntityType::id)
		.def_readwrite("symbol", &SGA::EntityType::symbol)
		.def_readwrite("parameters", &SGA::EntityType::parameters)
		.def_readwrite("action_ids", &SGA::EntityType::actionIds)
		.def_readwrite("required_technology_id", &SGA::EntityType::requiredTechnologyID)
		.def_readwrite("spawnable_entitytypes", &SGA::EntityType::spawnableEntityTypes)
		.def_readwrite("cost", &SGA::EntityType::cost)
		.def_readwrite("line_of_sight", &SGA::EntityType::lineOfSight)
		.def("can_execute_action", &SGA::EntityType::canExecuteAction, py::arg("actionTypeID"))
		.def("instantiate_entity", &SGA::EntityType::instantiateEntity, py::arg("entityID"))
		.def("get_param_max", &SGA::EntityType::getParamMax, py::arg("paramName"))
		.def("get_param_min", &SGA::EntityType::getParamMin, py::arg("paramName"))
		.def("get_parameter", &SGA::EntityType::getParameter, py::arg("id"));

	// ---- Player ----
	py::class_<SGA::Player>(m, "Player")
		.def("__copy__", [](const SGA::Player& self) {
		return SGA::Player(self);
			})
		.def("__deepcopy__", [](const SGA::Player& self, py::dict) {
				return SGA::Player(self);
			})
		.def(py::init<>())
		.def_readwrite("id", &SGA::Player::id)
		.def_readwrite("score", &SGA::Player::score)
		.def_readwrite("can_play", &SGA::Player::canPlay)
		.def_readwrite("parameters", &SGA::Player::parameters);

	// ---- Entity ----
	py::class_<SGA::Entity>(m, "Entity")
		.def("__copy__", [](const SGA::Entity& self) {
		return SGA::Entity(self);
			})
		.def("__deepcopy__", [](const SGA::Entity& self, py::dict) {
				return SGA::Entity(self);
			})
		.def(py::init<>())
		.def("init", &SGA::Entity::init, py::arg("type"), py::arg("entityID"))
		.def_readwrite("id", &SGA::Entity::id)
		.def_readwrite("owner_id", &SGA::Entity::ownerID)
		.def_readwrite("position", &SGA::Entity::position)
		.def_readwrite("line_of_sight_range", &SGA::Entity::lineOfSightRange)
		.def_readwrite("movement_speed", &SGA::Entity::movementSpeed)
		.def_readwrite("collision_radius", &SGA::Entity::collisionRadius)
		.def("get_entitytype", &SGA::Entity::getEntityType)
		.def("get_entitytype_id", &SGA::Entity::getEntityTypeID)		
		.def("is_neutral", &SGA::Entity::isNeutral)
		.def("get_action_info", &SGA::Entity::getActionInfo, py::arg("actionTypeID"))
		.def("get_attached_actions", &SGA::Entity::getAttachedActions)
		.def("set_action_ticks", &SGA::Entity::setActionTicks, py::arg("actionTypeID"), py::arg("tick"))
		.def("flag_remove", &SGA::Entity::flagRemove)
		.def("flagged", &SGA::Entity::flagged)
		.def("print_info", &SGA::Entity::printInfo)
		.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues, py::const_))
		.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues))
		.def("get_parameter", &SGA::Entity::getParameter, py::arg("paramName"))
		.def("get_parameter_at", &SGA::Entity::getParameterAt, py::arg("paramIdx"))
		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions, py::const_))
		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions))
		;

	// ---- GameType ----
	py::enum_<SGA::GameType>(m, "GameType")
		.value("TBS", SGA::GameType::TBS)
		.value("RTS", SGA::GameType::RTS);

	//---- GameDescription ----
	py::class_<SGA::GameDescription,std::shared_ptr<SGA::GameDescription>>(m, "GameDescription")
		.def_readwrite("action_categories", &SGA::GameDescription::actionCategories)
		.def_readwrite("entity_categories", &SGA::GameDescription::entityCategories)
		.def("get_action_types_ids", &SGA::GameDescription::getActionTypesIDs, py::arg("category"))
		.def("get_action_types", &SGA::GameDescription::getActionTypes, py::arg("category"), py::arg("gameInfo"))
		.def("is_from_category", &SGA::GameDescription::isFromCategory, py::arg("category"), py::arg("entityTypeId"))
	;

	// ---- EntityCategory ----
	py::enum_<SGA::EntityCategory>(m, "EntityCategory")
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
	py::enum_<SGA::ActionCategory>(m, "ActionCategory")
		.value("Null", SGA::ActionCategory::Null)
		.value("Move", SGA::ActionCategory::Move)
		.value("Attack", SGA::ActionCategory::Attack)
		.value("Heal", SGA::ActionCategory::Heal)
		.value("Gather", SGA::ActionCategory::Gather)
		.value("Spawn", SGA::ActionCategory::Spawn)
		.value("Research", SGA::ActionCategory::Research)
		.export_values()
		;

	// ---- GameState ----
	py::class_<SGA::GameState>(m, "GameState")
		//.def(py::init<SGA::GameState>())
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
		.def_readwrite("current_player", &SGA::GameState::currentPlayer)
		.def_readwrite("game_type", &SGA::GameState::gameType)
		.def_readwrite("is_game_over", &SGA::GameState::isGameOver)
		.def_readwrite("winner_player_id", &SGA::GameState::winnerPlayerID)
		.def_readwrite("current_tick", &SGA::GameState::currentTick)
		.def_readwrite("tick_limit", &SGA::GameState::tickLimit)
		.def_readwrite("fog_of_war_id", &SGA::GameState::fogOfWarId)
		.def_readwrite("fog_of_war_tile", &SGA::GameState::fogOfWarTile)
		.def_readwrite("board", &SGA::GameState::board)
		.def_readwrite("entities", &SGA::GameState::entities)
		.def_readwrite("players", &SGA::GameState::players)
		.def_readwrite("game_info", &SGA::GameState::gameInfo)

		.def_readwrite("researched_technologies", &SGA::GameState::researchedTechnologies)

		//Technologies
		.def("is_researched", &SGA::GameState::isResearched, py::arg("playerID"), py::arg("technologyID"), "Check if technology is researched")
		.def("can_research", &SGA::GameState::canResearch, py::arg("playerID"), py::arg("technologyID"), "Check if player can research an technology")
		.def("research_technology", &SGA::GameState::researchTechnology, py::arg("playerID"), py::arg("technologyID"), "Research technology")
		
		//Other
		.def("can_execute_action", py::overload_cast<const SGA::Entity&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Entity can execute a given actionType")
		.def("can_execute_action", py::overload_cast<const SGA::Player&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Entity can execute a given actionType")
		
		////Entities
		.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::automatic_reference,  "Get entity")
		.def("get_entity", py::overload_cast<SGA::Vector2f, float>(&SGA::GameState::getEntity), "Get entity")
		.def("get_entity", py::overload_cast<SGA::Vector2f>(&SGA::GameState::getEntity), "Get entity")
		.def("get_entity_at", &SGA::GameState::getEntityAt, py::arg("pos"), "Get entity at a position")
		.def("get_entity_const", &SGA::GameState::getEntityConst, py::arg("entityID"), "Get entity at a position")
		.def("add_entity", &SGA::GameState::addEntity, py::arg("type"), py::arg("playerID"), py::arg("position"), "Add entity to gamestate")
		
		////Grid
		.def("is_walkable", &SGA::GameState::isWalkable, py::arg("position"), "Checks if tile is occupied or the tile is walkable")
		.def("is_in_bounds", py::overload_cast<SGA::Vector2f>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")
		.def("is_in_bounds", py::overload_cast<SGA::Vector2i>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")

		////Player
		.def("get_player", py::overload_cast<int>(&SGA::GameState::getPlayer, py::const_),py::return_value_policy::reference, "Get player")
		.def("get_player", py::overload_cast<int>(&SGA::GameState::getPlayer), py::return_value_policy::reference, "Get player")
		
		.def("get_player_entities", py::overload_cast<int>(&SGA::GameState::getPlayerEntities, py::const_), "Gets the list of entities of the specified player.")
		.def("get_player_entities", py::overload_cast<int, SGA::EntityCategory>(&SGA::GameState::getPlayerEntities), "Gets the list of entities of the specified player.")
		
		.def("get_non_playerEntities", py::overload_cast<int, SGA::EntityCategory>(&SGA::GameState::getNonPlayerEntities), "Gets the list of entities that do not belong to the specified player.")
		
		.def("add_player", &SGA::GameState::addPlayer, py::arg("actionIds"), "Adds player to gamestate")
		.def("apply_fog_of_war", &SGA::GameState::applyFogOfWar, py::arg("playerID"), "Removes entities and hide tiles that are not visible from the point of view of the given player.")
		
		.def("set_rng_engine", &SGA::GameState::setRNGEngine, py::arg("engine"))
		.def("get_rng_engine", &SGA::GameState::getRNGEngine)

		//Print information
		.def("print_state_info", &SGA::GameState::printStateInfo,"Print all the entities of the current state")
		.def("print_board", py::overload_cast<int>(&SGA::GameState::printBoard, py::const_))
		.def("print_board", py::overload_cast<>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state applying fog")
		.def("print_entity_info", &SGA::GameState::printEntityInfo, py::arg("entityID"), "Print information of a specific entity")
		.def("print_action_info", &SGA::GameState::printActionInfo, py::arg("action"), "Print information of a specific action")
		;

	//---- Random Engine ----
	py::class_<std::mt19937>(m, "mt19937");

	

	// ---- TileType ----
	py::class_<SGA::TileType, std::shared_ptr<SGA::TileType>>(m, "TileType")
		.def(py::init<>())
		.def_readwrite("name", &SGA::TileType::name)
		.def_readwrite("symbol", &SGA::TileType::symbol)
		.def_readwrite("id", &SGA::TileType::id)
		.def_readwrite("is_walkable", &SGA::TileType::isWalkable)
		.def_readwrite("is_default_tile", &SGA::TileType::isDefaultTile)
		.def("to_tile", &SGA::TileType::toTile, py::arg("x"), py::arg("y"));

	// ---- Tile ----
	py::class_<SGA::Tile>(m, "Tile")
		.def(py::init<int, const SGA::TileType*, int, int>(), py::arg("typeID"), py::arg("tileType"), py::arg("x"), py::arg("y"))
		.def_readwrite("is_walkable", &SGA::Tile::isWalkable)
		.def_readwrite("blocks_sight", &SGA::Tile::blocksSight)
		.def_readwrite("position", &SGA::Tile::position)
		.def_readwrite("position", &SGA::Tile::position)
		
		.def("get_tile_type_id", &SGA::Tile::getTileTypeID);

	// ---- Parameter ----
	py::class_<SGA::Parameter>(m, "Parameter")
		.def(py::init<>())
		.def_readwrite("id", &SGA::Parameter::id)
		.def_readwrite("name", &SGA::Parameter::name)
		.def_readwrite("index", &SGA::Parameter::index)
		.def_readwrite("default_value", &SGA::Parameter::defaultValue)
		.def_readwrite("min_value", &SGA::Parameter::minValue)
		.def_readwrite("max_value", &SGA::Parameter::maxValue);

	// ---- ActionFlag ----
	py::enum_<SGA::ActionFlag>(m, "ActionFlag")
		.value("None", SGA::ActionFlag::None)
		.value("EndTickAction", SGA::ActionFlag::EndTickAction)
		.value("ContinuousAction", SGA::ActionFlag::ContinuousAction)
		.value("AbortContinuousAction", SGA::ActionFlag::AbortContinuousAction)
		.export_values();
	
	// ---- ActionInfo ----
	py::class_<SGA::ActionInfo>(m, "ActionInfo")
		.def(py::init<>())
		.def_readwrite("action_type_id", &SGA::ActionInfo::actionTypeID)
		.def_readwrite("last_executed_tick", &SGA::ActionInfo::lastExecutedTick)		;

	// ---- Action ----	
	py::class_<SGA::Action>(m, "Action")
		.def("__copy__", [](const SGA::Action& self) {
		return SGA::Action(self);
			})
		.def("__deepcopy__", [](const SGA::Action& self, py::dict) {
				return SGA::Action(self);
			})
		.def(py::init<const SGA::ActionType*>(), py::arg("actionType"))
		.def_readwrite("action_type_flags", &SGA::Action::actionTypeFlags)
		.def_readwrite("targets", &SGA::Action::targets)
		.def_readwrite("owner_id", &SGA::Action::ownerID)
		.def_readwrite("continuous_action_id", &SGA::Action::continuousActionID)
		.def_readwrite("elapsed_ticks", &SGA::Action::elapsedTicks)
		.def("execute", &SGA::Action::execute, py::arg("state"), py::arg("fm"))
		.def("validate", &SGA::Action::validate, py::arg("state"))
		.def("is_entity_action", &SGA::Action::isEntityAction)
		.def("is_player_action", &SGA::Action::isPlayerAction)
		.def("get_source_id", &SGA::Action::getSourceID)
		.def("get_actiontype_id", &SGA::Action::getActionTypeID)
		.def("get_actiontype", &SGA::Action::getActionType)
		.def_static("create_end_action", &SGA::Action::createEndAction, py::arg("playerID"))
		.def_static("create_abort_action", py::overload_cast<int,int,int>(&SGA::Action::createAbortAction))
		.def_static("create_abort_action", py::overload_cast<int,int>(&SGA::Action::createAbortAction))
		;

	// --- ActionSourceType ---
	py::enum_<SGA::ActionSourceType>(m, "ActionSourceType")
		.value("Entity", SGA::ActionSourceType::Entity)
		.value("Building", SGA::ActionSourceType::Player)
		.export_values();

	// --- ActionSourceTypeEnum ---
	py::enum_<SGA::TargetType::Type>(m, "TargetTypeEnum")
		.value("None", SGA::TargetType::Type::None)
		.value("Position", SGA::TargetType::Type::Position)
		.value("EntityType", SGA::TargetType::Type::EntityType)
		.value("Entity", SGA::TargetType::Type::Entity)
		.value("Technology", SGA::TargetType::Type::Technology)
		.value("ContinuousAction", SGA::TargetType::Type::ContinuousAction)
		.export_values();

	// ---- TargetType ----	
	py::class_<SGA::TargetType>(m, "TargetType")
		.def(py::init<>())
		.def_readwrite("type", &SGA::TargetType::type)
		.def_readwrite("sampling_method", &SGA::TargetType::samplingMethod)
		.def_readwrite("group_entity_types", &SGA::TargetType::groupEntityTypes)
		.def_readwrite("technology_types", &SGA::TargetType::technologyTypes)
		.def("get_type_string", &SGA::TargetType::getTypeString)
		.def("is_valid", &SGA::TargetType::isValid, py::arg("state"), py::arg("actionTarget"), py::arg("sourceActionTarget"))
		;

	// ---- SamplingMethod ----	
	py::class_<SGA::SamplingMethod, PySamplingMethod, std::shared_ptr<SGA::SamplingMethod>>(m, "SamplingMethod")
		.def(py::init<>())
		.def_readwrite("name", &SGA::SamplingMethod::name)
		.def("get_positions", py::overload_cast<const SGA::GameState&, const SGA::Vector2f&>(&SGA::SamplingMethod::getPositions, py::const_))		
		.def("get_positions", py::overload_cast<const SGA::GameState&>(&SGA::SamplingMethod::getPositions, py::const_))
		.def("get_entities", py::overload_cast<const SGA::GameState&, const SGA::Vector2f&, const std::unordered_set<int>&>(&SGA::SamplingMethod::getEntities, py::const_))
		.def("get_entities", py::overload_cast<const SGA::GameState&, const std::unordered_set<int>&>(&SGA::SamplingMethod::getEntities, py::const_))
		.def("validate_position", py::overload_cast<const SGA::GameState&, const SGA::Vector2f&, const SGA::Vector2f&>(&SGA::SamplingMethod::validatePosition, py::const_))
		.def("validate_position", py::overload_cast<const SGA::GameState&, const SGA::Vector2f&>(&SGA::SamplingMethod::validatePosition, py::const_))
		;

	// ---- Condition ----	
	py::class_<SGA::Condition, PyCondition, std::shared_ptr<SGA::Condition>>(m, "Condition")
		.def("isFullfilled", &SGA:: Condition::isFullfilled, py::arg("state"), py::arg("targets"))
		;
	
	// ---- Effect ----	
	py::class_<SGA::Effect, PyEffect, std::shared_ptr<SGA::Effect>>(m, "Effect")
		.def("execute", &SGA::Effect::execute, py::arg("state"), py::arg("fm"), py::arg("targets"))
		;
	
	// --- ActionTargetEnum ---
	py::enum_<SGA::ActionTarget::Type>(m, "ActionTargetEnum")
		.value("Position", SGA::ActionTarget::Type::Position)
		.value("EntityReference", SGA::ActionTarget::Type::EntityReference)
		.value("PlayerReference", SGA::ActionTarget::Type::PlayerReference)
		.value("EntityTypeReference", SGA::ActionTarget::Type::EntityTypeReference)
		.value("TechnologyReference", SGA::ActionTarget::Type::TechnologyReference)
		.value("ContinuousActionReference", SGA::ActionTarget::Type::ContinuousActionReference)
		.value("TileTypeReference", SGA::ActionTarget::Type::TileTypeReference)
		.export_values();

	// ---- ActionTarget ----	
	py::class_<SGA::ActionTarget>(m, "ActionTarget")
		.def_static("create_position_action_target", &SGA::ActionTarget::createPositionActionTarget, py::arg("position"))
		.def_static("create_entity_action_target", &SGA::ActionTarget::createEntityActionTarget, py::arg("entityID"))
		.def_static("create_player_action_target", &SGA::ActionTarget::createPlayerActionTarget, py::arg("playerID"))
		.def_static("create_technology_entity_action_target", &SGA::ActionTarget::createTechnologyEntityActionTarget, py::arg("technologyID"))
		.def_static("create_continuous_action_target", &SGA::ActionTarget::createContinuousActionActionTarget, py::arg("continuousActionID"))
		.def_static("create_entity_type_action_target", &SGA::ActionTarget::createEntityTypeActionTarget, py::arg("entityTypeID"))
		.def_static("create_tile_type_action_target", &SGA::ActionTarget::createTileTypeActionTarget, py::arg("entityTypeID"))

		.def("get_entity_const", &SGA::ActionTarget::getEntityConst, py::arg("state"))
		.def("get_entity", &SGA::ActionTarget::getEntity, py::arg("state"))
		.def("get_player", &SGA::ActionTarget::getPlayer, py::arg("state"))
		.def("get_player_const", &SGA::ActionTarget::getPlayerConst, py::arg("state"))
		.def("get_entity_type", &SGA::ActionTarget::getEntityType, py::arg("state"))
		.def("get_tile_type", &SGA::ActionTarget::getTileType, py::arg("state"))
		.def("get_spawneable_entities", &SGA::ActionTarget::getSpawneableEntities, py::arg("state"))

		.def("get_position", &SGA::ActionTarget::getPosition, py::arg("state"))
		.def("get_technology_id", &SGA::ActionTarget::getTechnologyID)
		.def("get_player_id", py::overload_cast<const SGA::GameState&>(&SGA::ActionTarget::getPlayerID, py::const_))
		.def("get_player_id", py::overload_cast<>(&SGA::ActionTarget::getPlayerID, py::const_))
		.def("get_entity_id", &SGA::ActionTarget::getEntityID)
		.def("get_continuous_action_id", &SGA::ActionTarget::getContinuousActionID)
		.def("get_type", &SGA::ActionTarget::getType)
		.def("get_type_string", &SGA::ActionTarget::getTypeString)
		.def("get_value_string", &SGA::ActionTarget::getValueString, py::arg("state"))

		.def_static("is_valid_with_targets", &SGA::ActionTarget::isValidWithTargets, py::arg("state"), py::arg("actionType"), py::arg("actionTargets"))
		.def("is_valid", &SGA::ActionTarget::isValid, py::arg("state"))
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

	// ---- TechnologyTreeCollection ----	
	py::class_<SGA::TechnologyTreeCollection, std::shared_ptr<SGA::TechnologyTreeCollection>>(m, "TechnologyTreeCollection")
		.def_readwrite("technology_tree_types", &SGA::TechnologyTreeCollection::technologyTreeTypes)
		.def("get_technology", &SGA::TechnologyTreeCollection::getTechnology, py::arg("technologyID"))
		.def("get_technology_type_id", &SGA::TechnologyTreeCollection::getTechnologyTypeID, py::arg("technologyName"))
		;
	// ---- TechnologyTreeType ----	
	py::class_<SGA::TechnologyTreeType, std::shared_ptr<SGA::TechnologyTreeType>>(m, "TechnologyTreeType")
		.def_readwrite("technology_tree_name", &SGA::TechnologyTreeType::technologyTreeName)
		.def_readwrite("technologies", &SGA::TechnologyTreeType::technologies)
		.def("getTechnology_node", &SGA::TechnologyTreeType::getTechnologyNode, py::arg("technologyID"))
		.def("findTechnology_node", &SGA::TechnologyTreeType::findTechnologyNode, py::arg("technologyID"))
		.def("getTechnology_node_id", &SGA::TechnologyTreeType::getTechnologyNodeID, py::arg("technologyName"))
		.def("have_technology_node", &SGA::TechnologyTreeType::haveTechnologyNode, py::arg("technologyName"))
		;
	// ---- TechnologyTreeNode ----	
	py::class_<SGA::TechnologyTreeNode, std::shared_ptr<SGA::TechnologyTreeNode>>(m, "TechnologyTreeNode")
		.def_readwrite("name", &SGA::TechnologyTreeNode::name)
		.def_readwrite("description", &SGA::TechnologyTreeNode::description)
		.def_readwrite("id", &SGA::TechnologyTreeNode::id)
		.def_readwrite("parent_ids", &SGA::TechnologyTreeNode::parentIDs)
		.def_readwrite("cost", &SGA::TechnologyTreeNode::cost)
		.def_readwrite("continuousActionTime", &SGA::TechnologyTreeNode::continuousActionTime)
		;
	// ---- Grid2D<Tile> ----
	py::class_<SGA::Grid2D<SGA::Tile>>(m, "BoardTiles")
		.def(py::init<int, int, SGA::Tile>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
		.def_readwrite("grid", &SGA::Grid2D<SGA::Tile>::grid)
		.def("get", [](const SGA::Grid2D<SGA::Tile>& b, int y, int x) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<SGA::Tile>::getWidth)
		.def("get_height", &SGA::Grid2D<SGA::Tile>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		;
	// ---- Grid2D<TileType> ----
	py::class_<SGA::Grid2D<std::shared_ptr<SGA::TileType>>>(m, "BoardTileTypes")
		.def(py::init<int, int, std::shared_ptr<SGA::TileType>>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
		.def_readwrite("grid", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::grid)
		.def("get", [](const SGA::Grid2D<std::shared_ptr<SGA::TileType>>& b, int y, int x) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getWidth)
		.def("get_height", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		;
	 
	// ---- GameRunner ----
	py::class_<SGA::GameRunner>(m, "GameRunner")
		.def("initialize_agents", &SGA::GameRunnerPy::initializeAgents)
		.def("play",
			[](SGA::GameRunner& a, std::vector<std::shared_ptr<SGA::Agent>> newAgents, int seed = 0)
			{
				py::scoped_ostream_redirect stream(
					std::cout,                               // std::ostream&
					py::module_::import("sys").attr("stdout") // Python output
				);

				std::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
				for (auto& agent : newAgents)
				{
					auto seed = seedDist(rngEngine);
					// Ignore human player
					if (agent != nullptr)
					{
						agent->setSeed(seed);
					}
				}

				std::cout << "Run GUI" << std::endl;
				py::gil_scoped_release release;
				a.play(newAgents);
				py::gil_scoped_acquire acquire;
			}
		)
		.def("play",
			[](SGA::GameRunner& a, py::list agents, int seed=0)
			{
				py::scoped_ostream_redirect stream(
					std::cout,                               // std::ostream&
					py::module_::import("sys").attr("stdout") // Python output
				);

				std::vector<std::shared_ptr<SGA::Agent>> newAgents;
				for (auto& agent : agents)
				{
					if (pybind11::str(agent,true).check())
					{
						newAgents.emplace_back(SGA::AgentFactory::get().createAgent(agent.cast<std::string>()));
					}
					else
					{
						auto castedAgent = agent.cast<std::shared_ptr<PyAgent>>();
						newAgents.emplace_back(castedAgent);
					}
				}

				std::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
				for (auto& agent : newAgents)
				{
					auto seed = seedDist(rngEngine);
					// Ignore human player
					if (agent != nullptr)
					{
						agent->setSeed(seed);
					}
				}

				std::cout << "Run GUI" << std::endl;
				py::gil_scoped_release release;
				a.play(newAgents);
				py::gil_scoped_acquire acquire;
			}
		)
		.def("run",
			[](SGA::GameRunner& a, py::list agents, int seed=0)
			{
				py::scoped_ostream_redirect stream(
					std::cout,                               // std::ostream&
					py::module_::import("sys").attr("stdout") // Python output
				);

				std::vector<std::shared_ptr<SGA::Agent>> newAgents;
				for (auto& agent : agents)
				{
					if (pybind11::str(agent, true).check())
					{
						newAgents.emplace_back(SGA::AgentFactory::get().createAgent(agent.cast<std::string>()));
					}
					else
					{
						auto castedAgent = agent.cast<std::shared_ptr<PyAgent>>();
						newAgents.emplace_back(castedAgent);
					}
				}

				std::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
				for (auto& agent : newAgents)
				{
					auto seed = seedDist(rngEngine);
					// Ignore human player
					if (agent != nullptr)
					{
						agent->setSeed(seed);
					}
				}
				std::cout << "Run arena" << std::endl;
				py::gil_scoped_release release;				
				a.run(newAgents);
				py::gil_scoped_acquire acquire;
			}
		)
		.def("run",
			[](SGA::GameRunner& a, std::vector<std::shared_ptr<SGA::Agent>> newAgents, int seed=0)
			{
				py::scoped_ostream_redirect stream(
					std::cout,                               // std::ostream&
					py::module_::import("sys").attr("stdout") // Python output
				);

				std::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
				for (auto& agent : newAgents)
				{
					auto seed = seedDist(rngEngine);
					// Ignore human player
					if (agent != nullptr)
					{
						agent->setSeed(seed);
					}
				}
				std::cout << "Run arena" << std::endl;
				py::gil_scoped_release release;				
				a.run(newAgents);
				py::gil_scoped_acquire acquire;
			}
		)
		.def("reset", py::overload_cast<int>(&SGA::GameRunner::reset),py::arg("levelID"))
		.def("reset", py::overload_cast<>(&SGA::GameRunner::reset))
		.def("step", &SGA::GameRunner::step, py::arg("actions"))
		.def("render", &SGA::GameRunner::render)
		.def("get_gamestate", &SGA::GameRunner::getGameState)
		;
	
	// ---- GameConfig ----
	py::class_<SGA::GameConfig>(m, "GameConfig")
		.def_readwrite("game_type", &SGA::GameConfig::gameType)

		.def_readwrite("tick_limit", &SGA::GameConfig::tickLimit)
		.def_readwrite("num_players", &SGA::GameConfig::numPlayers)
		.def_readwrite("tile_types", &SGA::GameConfig::tileTypes)

		//.def_property_readonly("forward_model", [](const SGA::GameConfig& config) {return config.forwardModel.get(); }, py::return_value_policy::reference_internal)

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
		.def_readwrite("yaml_path", &SGA::GameInfo::yamlPath)

		//.def_readwrite("parameter_id_lookup", &SGA::GameInfo::parameterIDLookup)
		//.def_readwrite("player_parameter_types", &SGA::GameInfo::playerParameterTypes)
		//.def_readwrite("player_Spawnable_types", &SGA::GameInfo::playerSpawnableTypes)
		//.def_readwrite("entity_types", &SGA::GameInfo::entityTypes)
		//.def_readwrite("action_types", &SGA::GameInfo::actionTypes)
		//.def_readwrite("tile_types", &SGA::GameInfo::tileTypes)

		.def_readwrite("technology_tree_collection", &SGA::GameInfo::technologyTreeCollection)
		.def_readwrite("entity_groups", &SGA::GameInfo::entityGroups)

		.def_readwrite("game_description", &SGA::GameInfo::gameDescription)

		.def("get_game_description", &SGA::GameInfo::getGameDescription)
		.def("get_entity_type", &SGA::GameInfo::getEntityType, py::arg("entityTypeID"))
		.def("get_parameter_global_id", &SGA::GameInfo::getParameterGlobalID, py::arg("parameterName"))
		.def("get_parameter_type", &SGA::GameInfo::getParameterType, py::arg("entityTypeID"), py::arg("globalParameterID"))
		.def("check_entity_have_parameter", &SGA::GameInfo::checkEntityHaveParameter, py::arg("entityTypeID"), py::arg("parameterName"))
		.def("get_action_type_const", &SGA::GameInfo::getActionTypeConst, py::arg("actionTypeID"))
		.def("get_action_type", &SGA::GameInfo::getActionType, py::arg("typeID"))
		.def("get_action_type_id", &SGA::GameInfo::getActionTypeID, py::arg("actionName"))
		.def("get_player_parameter", &SGA::GameInfo::getPlayerParameter, py::arg("id"))
		.def("get_tile_type", &SGA::GameInfo::getTileType, py::arg("tileTypeID"))

		;

	// ---- GameObserver ----
	py::class_<SGA::GameObserver>(m, "GameObserver");


	// ---- Heuristic ----
	py::class_<SGA::MinimizeDistanceHeuristic>(m, "MinimizeDistanceHeuristic")
		.def(py::init<>())
		.def_static("get_name", &SGA::MinimizeDistanceHeuristic::getName)
		.def("evaluate_gamestate", &SGA::MinimizeDistanceHeuristic::evaluateGameState, py::arg("forwardModel"), py::arg("gameState"), py::arg("playerID"));

	// ---- Action Assignment ----
	py::class_<SGA::ActionAssignment>(m, "ActionAssignment")
		.def("__copy__", [](const SGA::ActionAssignment& self) {
		return SGA::ActionAssignment(self);
			})
		.def("__deepcopy__", [](const SGA::ActionAssignment& self, py::dict) {
				return SGA::ActionAssignment(self);
			})
		.def(py::init<>())
		.def("assign_action_or_replace", &SGA::ActionAssignment::assignActionOrReplace, py::arg("newAction"))
		.def("merge", &SGA::ActionAssignment::merge, py::arg("merge"))
		.def("clear", &SGA::ActionAssignment::clear)
		.def("get_entity_action", &SGA::ActionAssignment::getEntityAction, py::arg("entityID"))
		.def("get_player_action", &SGA::ActionAssignment::getPlayerAction, py::arg("playerID"))
		.def("get_assignment_count", &SGA::ActionAssignment::getAssignmentCount)
		.def("get_entity_actions", &SGA::ActionAssignment::getEntityActions)
		.def("get_player_actions", &SGA::ActionAssignment::getPlayerActions)
		.def_static("from_single_action", &SGA::ActionAssignment::fromSingleAction, py::arg("a"));

	// ---- Forward model ----
	py::class_<SGA::EntityForwardModel>(m, "EntityForwardModel");
		//.def("generateActions", py::overload_cast<const SGA::GameState&, int>(&SGA::EntityForwardModel::generateActions, py::const_));
		//.def("generateActions2", &SGA::EntityForwardModel::generateActions2, py::arg("state"), py::arg("playerID"));
		
	py::class_<SGA::TBSForwardModel, SGA::EntityForwardModel>(m, "TBSForwardModel")
		.def(py::init<>())
		.def("generate_actions", &SGA::TBSForwardModel::generateActions2, py::arg("state"), py::arg("playerID"))
		//.def("generate_actions", &SGA::TBSForwardModel::generateActions, py::arg("state"), py::arg("playerID"))
		//.def("generateActions", py::overload_cast<const SGA::GameState&, int>(&SGA::TBSForwardModel::generateActions, py::const_))
		.def("advance_gamestate_const", py::overload_cast<SGA::GameState&, const SGA::Action&>(&SGA::TBSForwardModel::advanceGameState, py::const_))
		//.def("generateActions", py::overload_cast<const SGA::GameState& , int>(&SGA::TBSForwardModel::generateActions, py::const_));
		//.def("generateActions", &SGA::TBSForwardModel::generateActions, py::arg("state"), py::arg("playerID"), py::const_);
		//.def("generateActions", py::overload_cast<SGA::GameState& , int>(&SGA::TBSForwardModel::generateActions, py::const_));
		.def("advance_gamestate",
			[](SGA::EntityForwardModel& fm, SGA::GameState& state, SGA::Action& action)
			{
				SGA::GameState newGS = state;
				SGA::Action newAction = action;
				//py::gil_scoped_acquire acquire;
				//py::scoped_ostream_redirect stream(
				//	std::cout,                               // std::ostream&
				//	py::module_::import("sys").attr("stdout") // Python output
				//);

				fm.advanceGameState(newGS, newAction);
			}
		)
		/*.def("advanceGameState", py::overload_cast<SGA::GameState&, const SGA::Action&>(&SGA::TBSForwardModel::advanceGameState, py::const_))*/;

	// ---- Agent ----
	py::class_<SGA::Agent, PyAgent, std::shared_ptr<SGA::Agent>/* <--- trampoline*/>(m, "Agent")
		.def(py::init<>())
		.def("computeAction", &SGA::Agent::computeAction, py::return_value_policy::reference)
		.def("get_player_id", &SGA::Agent::getPlayerID);

}