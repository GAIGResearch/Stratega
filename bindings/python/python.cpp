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
#include <fstream>
#include <sstream>
#include <filesystem>
#include <Stratega/Logging/Log.h>

#include <Stratega/Utils/Timer.h>

namespace py = pybind11;
class PythonAgent;

// STL
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Action>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Player>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity*>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Agent*>);
PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SGA::Agent>>);
PYBIND11_MAKE_OPAQUE(std::vector<PythonAgent>);


class PyCondition : public SGA::Condition {
public:
	/* Inherit the constructors */
	using SGA::Condition::Condition;

	using SGA::Condition::expr;

	/* Trampoline (need one for each virtual function) */
	bool isFullfiled(const SGA::GameState& state, const std::vector<SGA::ActionTarget>& targets)const override {
		PYBIND11_OVERRIDE_PURE_NAME(
			bool, /* Return type */
			SGA::Condition,      /* Parent class */
			"is_fullfiled",
			isFullfiled,          /* Name of function in C++ (must match Python name) */
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
		PYBIND11_OVERRIDE_PURE_NAME(
			std::vector<SGA::Vector2i>, /* Return type */
			SGA::SamplingMethod,      /* Parent class */
			"get_positions",
			getPositions,          /* Name of function in C++ (must match Python name) */
			gameState,
			position/* Argument(s) */
		);
	}

	/* Trampoline (need one for each virtual function) */
	std::vector<int>getEntities(const SGA::GameState& gameState, const SGA::Vector2f& position, const std::unordered_set<int>& entityTypeIDs)const override
	{
		PYBIND11_OVERRIDE_PURE_NAME(
			std::vector<int>, /* Return type */
			SGA::SamplingMethod,      /* Parent class */
			"get_entities",
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
	void execute(SGA::GameState& state, const SGA::ForwardModel& fm, const std::vector<SGA::ActionTarget>& targets)const override {
		PYBIND11_OVERRIDE_PURE(
			void, /* Return type */
			SGA::Effect,      /* Parent class */
			execute,          /* Name of function in C++ (must match Python name) */
			state,
			&fm,
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
	SGA::ActionAssignment computeAction(SGA::GameState state, const SGA::ForwardModel& forwardModel, SGA::Timer timer) override
	{
		PYBIND11_OVERRIDE_PURE_NAME(
			SGA::ActionAssignment,
			SGA::Agent,
			"compute_action",
			computeAction,
			state,
			&forwardModel,
			timer
		);
	}

	/* Trampoline (need one for each virtual function) */
	void init(SGA::GameState state, const SGA::ForwardModel& forwardModel, SGA::Timer timer) override
	{
		PYBIND11_OVERRIDE_PURE_NAME(
			void,
			SGA::Agent,
			"init",
			init,
			state,
			&forwardModel,
			timer
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

 void setDefaultLogger(std::string logPath)
 {
	 SGA::Log::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
 }

 Arena createArena(const SGA::GameConfig* gameConfig)
 {
	 return Arena(*gameConfig);
 }

PYBIND11_MODULE(stratega, m)
{
	py::options options;
	options.enable_function_signatures();
	options.enable_user_defined_docstrings();
	
    m.doc() = "Stratega python bindings"; // optional module docstring

	m.def("load_config", &loadConfig, "Loads game config", py::arg("path"));
	m.def("create_runner", &createRunner, "Create game runner", py::arg("gameConfig"));
	m.def("create_arena", &createArena, "Create game aren", py::arg("gameConfig"));
	m.def("generate_agents", &generateAgents, "Generate agents", py::arg("gameConfig"));
	m.def("initialize_agents", &initializeAgents, "Initialize agents", py::arg("agents"), py::arg("seed"));
	m.def("set_default_logger", &setDefaultLogger, "Set default logger", py::arg("logPath"));
	m.def("load_levels_from_yaml", &SGA::loadLevelsFromYAML, "Load Levels definitions  from YAML", py::arg("fileMapsPath"), py::arg("config"));
 	
	// ---- STL binds----
	//Basic list
	py::bind_vector<std::vector<double>>(m, "DoubleList");
	py::bind_vector<std::vector<int>>(m, "IntList");
	py::bind_vector<std::vector<std::string>>(m, "StringList");

	py::bind_vector<std::vector<SGA::Player>>(m, "PlayerList");
	py::bind_vector<std::vector<SGA::Entity>>(m, "EntityList");
	py::bind_vector<std::vector<SGA::Entity*>>(m, "EntityPointerList");

	py::bind_vector<std::vector<std::shared_ptr<SGA::Agent>>>(m, "AgentList");

	py::bind_vector<std::vector<SGA::ActionTarget>>(m, "ActionTargetList");

	py::bind_vector<std::vector<std::shared_ptr<SGA::EntityType>>>(m, "EntityTypetList");
	py::bind_vector<std::vector<SGA::EntityPlacement>>(m, "EntityPlacementList");

	//Technologies
	py::bind_map<std::unordered_map<int, SGA::TechnologyTreeNode>>(m, "TechnologiesMap");
	py::bind_map<std::unordered_map<int, std::vector<int>>>(m, "ResearchedTechnologiesMap");
	py::bind_map<std::unordered_map<int, int>>(m, "TechnologiesCountsMap");

	//Types
	py::bind_map<std::unordered_map<int, SGA::TileType>>(m, "TileTypesMap");
	py::bind_map<std::unordered_map<int, SGA::EntityType>>(m, "EntityTypesMap");
	py::bind_map<std::unordered_map<int, SGA::ActionType>>(m, "ActionTypesMap");

	//Actions stuff:
	py::bind_vector<std::vector<std::pair<SGA::TargetType, std::vector<std::shared_ptr<SGA::Condition>>>>>(m, "ActionTargetsList");
	py::bind_vector<std::vector<std::shared_ptr<SGA::Condition>>>(m, "ConditionsList");
	py::bind_vector<std::vector<std::shared_ptr<PyCondition>>>(m, "PyConditionsList");
	py::bind_vector<std::vector<std::shared_ptr<PyEffect>>>(m, "PyEffectsList");
	py::bind_vector<std::vector<std::shared_ptr<SGA::Effect>>>(m, "EffectsList");

	//ForwardModel
	py::bind_vector<std::vector<SGA::OnTickEffect>>(m, "OnTickEffectsList");
	py::bind_vector<std::vector<SGA::OnEntitySpawnEffect>>(m, "OnEntitySpawnEffectList");

	//Parameters
	py::bind_map<std::unordered_map<std::string, SGA::ParameterID>>(m, "ParametersMap");
	py::bind_map<std::unordered_map<SGA::ParameterID, SGA::Parameter>>(m, "PlayerParameterTypesMap");
	py::bind_map<std::unordered_map<std::string, double>>(m, "ParameterValueMap");

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
		.def("__getitem__", [](const std::vector<SGA::Action>& v, int index)
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
	py::class_<SGA::ActionType, std::shared_ptr<SGA::ActionType>>(m, "ActionType", "Contains the definition of an action. Which type is the source SGA::ActionSourceType , a set effects,preconditions and a list of action targets linked to their target conditions.If the action is continuous it will have a set of effects as events:OnStart, OnTick, OnComplete, OnAbort and the list of conditions that triggers the completed action.")
		.def(py::init<>())
		.def_readwrite("name", &SGA::ActionType::name, "Name of this action type.")
		.def_readwrite("source_type", &SGA::ActionType::sourceType, "Source of this action (entity, player...)")
		.def_readwrite("id", &SGA::ActionType::id, "Unique ID of this action type")
		.def_readwrite("cooldown_ticks", &SGA::ActionType::cooldownTicks, "Number of ticks/turn that must happen between two uses of this action.")
		.def_readwrite("action_targets", &SGA::ActionType::actionTargets, "List of target types of this action with their conditions. Each target is a pair <target, list<conditions>>. These are tier 2 conditions (i.e. situational), to be checked during action generation.")
		.def_readwrite("preconditions", &SGA::ActionType::preconditions, "List of preconditions for this action to be run. These are tier 1 conditions: if they can't be fulfilled the action is not possible. If the conditions are true, the action may be possible based on situational circumstances.  (Example: having a resource to create an entity but no space in the board to spawn it).")
		.def_readwrite("effects", &SGA::ActionType::effects, " List of effects this action causes when executed. This list is populated only for non continuous actions.")
		.def_readwrite("is_continuous", &SGA::ActionType::isContinuous, "Indicates if action is continuous: if the execution of this action extends beyond the tick it was started.")
		.def_readwrite("trigger_complete", &SGA::ActionType::triggerComplete, "Continuous actions finish when certain conditions are met. This vector indicates those conditions.")
		.def_readwrite("on_start", &SGA::ActionType::OnStart, "For continuous actions, list of effects that are applied when this action starts.")
		.def_readwrite("on_tick", &SGA::ActionType::OnTick, "For continuous actions, list of effects that are applied on every tick/turn. This effect is *not* applied on the tick this action starts, but it *will* also be applied in the tick it ends.")
		.def_readwrite("on_complete", &SGA::ActionType::OnComplete, "For continuous actions, list of effects that are applied when this action ends.")
		.def_readwrite("on_abort", &SGA::ActionType::OnAbort, "For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in the game tick they have been aborted.")
		.def("get_target_conditions", &SGA::ActionType::getTargetConditions, py::arg("searchingTarget"), "Returns a list of conditions linked to the searched target.");

	// ---- Entity type ----
	py::class_<SGA::EntityType, std::shared_ptr<SGA::EntityType>>(m, "EntityType", "Contains the definition of the entity type. This information includes a list of parameters definitions, the actions that can execute, required technologies to spawn a entity, a list of spawnable entity types and a list of cost assigned to this entity type.")
		.def(py::init<>())
		.def_readwrite("name", &SGA::EntityType::name, "Name of this type, as defined in the YAML file.")
		.def_readwrite("id", &SGA::EntityType::id, "Unique entity type ID")
		.def_readwrite("symbol", &SGA::EntityType::symbol, "Symbol for this entity used in YAML for level placement.")
		.def_readwrite("parameters", &SGA::EntityType::parameters, "Map of parameters of this entity. Key is an integer (ID of the parameter) and value is the paramter object. The parameter defines the min, max and default value this parameter can take.")
		.def_readwrite("action_ids", &SGA::EntityType::actionIds, "List of action IDs this entity can execute.")
		.def_readwrite("required_technology_id", &SGA::EntityType::requiredTechnologyID, "ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.")
		.def_readwrite("spawnable_entitytypes", &SGA::EntityType::spawnableEntityTypes, "List of entity types this entity can spwan in the game.")
		.def_readwrite("cost", &SGA::EntityType::cost, "List of parameters and values that must be spent to spawn/build/create this unit.")
		.def_readwrite("continuous_action_time", &SGA::EntityType::continuousActionTime, "Continuous Action Time")
		.def_readwrite("line_of_sight_range", &SGA::EntityType::lineOfSightRange, "Range of the line of sight of this unit.")
		.def("get_parameter", &SGA::EntityType::getParameter, py::arg("id"), "Returns a SGA::Parameter reference that contains the parameter defintion")
		.def("can_execute_action", &SGA::EntityType::canExecuteAction, py::arg("actionTypeID"), "Checks if this entity type is allowed to execute an action type")
		.def("instantiate_entity", &SGA::EntityType::instantiateEntity, py::arg("entityID"), "Generate a new empty instance of this entity type ")
		.def("get_param_max", &SGA::EntityType::getParamMax, py::arg("paramName"), "Returns the maximum value of a given parameter")
		.def("get_param_min", &SGA::EntityType::getParamMin, py::arg("paramName"), " Returns the minimum value of a given parameter")
		;

	// ---- Player ----
	py::class_<SGA::Player>(m, "Player")
		.def("__copy__", [](const SGA::Player& self) {
		return SGA::Player(self);
			})
		.def("__deepcopy__", [](const SGA::Player& self, py::dict) {
				return SGA::Player(self);
			})
		.def(py::init<>())

		.def_readwrite("id", &SGA::Player::id, "ID of this player, unique for players.")
		//.def_readwrite("score", &SGA::Player::score)
		.def_readwrite("can_play", &SGA::Player::canPlay, "Indicates if this player can play at this time.")
		.def_readwrite("continuous_action", &SGA::Player::continuousAction, "Array of actions currently being executed by this player")
		.def_readwrite("parameters", &SGA::Player::parameters, "List of parameter values. Use GameState.getPlayerParameter(...) functions to retrieve this.")
		.def_readwrite("attached_actions", &SGA::Player::attachedActions, "Actions that this player can execute in this game.")

		.def("can_execute_action", &SGA::Player::canExecuteAction, py::arg("actionTypeID"), "Indicates if a given action type can be executed by this player.")
		.def("get_action_info", &SGA::Player::getActionInfo, py::arg("actionTypeID"), " Returns the ActionInfo of an action type this player can execute.")
		;

	// ---- Entity ----
	py::class_<SGA::Entity>(m, "Entity", "Is an object that interacts with the game and responds to player input or other entities. This entity can be assigned to a player performing a set of actions per tick/turn. It has a list of parameters that store real values. If the player wants to check the SGA::EntityType it can search it using the assigned typeID.")
		.def("__copy__", [](const SGA::Entity& self) {
		return SGA::Entity(self);
			})
		.def("__deepcopy__", [](const SGA::Entity& self, py::dict) {
				return SGA::Entity(self);
			})
		.def(py::init<>())
		.def("init", &SGA::Entity::init, py::arg("type"), py::arg("entityID"), "Initializes the entity with a given ID. Creates default attached actions and parameters list from the entity type.")

		.def_readwrite("id", &SGA::Entity::id, "ID of this entity in the game.")
		.def_readwrite("owner_id", &SGA::Entity::ownerID, "ID of the player who owns this entity.")
		.def_readwrite("position", &SGA::Entity::position, "Position of the entity in the board.")
		.def_readwrite("line_of_sight_range", &SGA::Entity::lineOfSightRange, "Range of the line of sight of this entity.")
		.def_readwrite("movement_speed", &SGA::Entity::movementSpeed, "Movement speed for this entity (RTS)")
		.def_readwrite("path", &SGA::Entity::path, "Path that this entity is following (RTS)")
		.def_readwrite("collision_radius", &SGA::Entity::collisionRadius, "Collision radius for this entity (RTS)")

		.def("get_entitytype", &SGA::Entity::getEntityType, "Return entity type")
		.def("get_entitytype_id", &SGA::Entity::getEntityTypeID, "Return entity type id")
		.def("is_neutral", &SGA::Entity::isNeutral, "Indicates if this unit is neutral (i.e. does not belong to any playing faction).")
		.def("get_action_info", &SGA::Entity::getActionInfo, py::arg("actionTypeID"), "Returns a SGA::ActionInfo object linked to action type,")
		.def("get_attached_actions", &SGA::Entity::getAttachedActions, "Returns the actions attached to this entity.")
		.def("get_action_types", &SGA::Entity::getActionTypes, py::arg("gameInfo"), "Retrieves the list of action types this entity can execute.")
		.def("set_action_ticks", &SGA::Entity::setActionTicks, py::arg("actionTypeID"), py::arg("tick"), "Sets the last executed game ticks to the tick passed by parameter for the action corresponding to the type received.")
		.def("flag_remove", &SGA::Entity::flagRemove, "Flags this entity to be removed by the game engine.")
		.def("flagged", &SGA::Entity::flagged, "Checks if this entity should be removed by the game engine.")

		.def("print_info", &SGA::Entity::printInfo, "Prints information about the entity, parameters and actions")
		.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues, py::const_), "Gets the list of parameters values. Modifiable.")
		.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues), "Gets the list of parameters values.")
		.def("get_entity_parameter_names", &SGA::Entity::getEntityParameterNames, "Gets the list of parameters names.")
		.def("get_entity_parameters", &SGA::Entity::getEntityParameters, " Gets a map with all pairs <parameter,value>")
		.def("get_parameter", &SGA::Entity::getParameter, py::arg("paramName"), "Gets the value of a specific parameter, by name ")
		.def("get_parameter_at", &SGA::Entity::getParameterAt, py::arg("paramIdx"), "Gets a specific parameters, by index ")

		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions, py::const_), "Gets the list of continuous actions attached to this entity. Modifiable.")
		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions), "Gets the list of continuous actions attached to this entity. ")
		;

	// ---- GameType ----
	py::enum_<SGA::GameType>(m, "GameType")
		.value("TBS", SGA::GameType::TBS)
		.value("RTS", SGA::GameType::RTS);

	//---- GameDescription ----
	py::class_<SGA::GameDescription,std::shared_ptr<SGA::GameDescription>>(m, "GameDescription")
		.def_readwrite("action_categories", &SGA::GameDescription::actionCategories, "//List of action IDs grouped by action categories")
		.def_readwrite("entity_categories", &SGA::GameDescription::entityCategories, "//List of entity IDs grouped by entity categories")
		.def("get_action_types_ids", &SGA::GameDescription::getActionTypesIDs, py::arg("category"), "Returns all the actiontypes IDs of the selected action category.")
		.def("get_action_types", &SGA::GameDescription::getActionTypes, py::arg("category"), py::arg("gameInfo"), "Returns all the actiontypes IDs of the selected action category.")
		.def("is_from_category", &SGA::GameDescription::isFromCategory, py::arg("category"), py::arg("entityTypeId"), "Checks if a given entity type ID belongs to a given category")

		.def_static("to_string", py::overload_cast<SGA::ActionCategory>(&SGA::GameDescription::toString))
		.def_static("to_string", py::overload_cast<SGA::EntityCategory>(&SGA::GameDescription::toString))
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

	// ---- GameState ----
	py::class_<SGA::GameState>(m, "GameState", "Contains the game data without any logic, offering access to the current board, a list of player and their units.  If the agent want access to the definition of entity types, actions or game config yaml  it should access to SGA::GameInfo")
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
		//.def_readwrite("current_player", &SGA::GameState::currentPlayer, "ID of the current player (TBS only)")
		.def_readwrite("game_type", &SGA::GameState::gameType, "Type of game being played (TBS, RTS...), as defined SGA::GameType")
		.def_readwrite("is_game_over", &SGA::GameState::isGameOver, "Indicates if the game is over.")
		.def_readwrite("winner_player_id", &SGA::GameState::winnerPlayerID, "The ID of the player who won the game if the game is over. Value is -1 if game not over yet. ")
		.def_readwrite("current_tick", &SGA::GameState::currentTick, "Current turn (for turn based games) or frame (real-time) of the game.")
		.def_readwrite("tick_limit", &SGA::GameState::tickLimit, "Number of turns (TBS) or frames (RTS) when the game will finish.")
		.def_readwrite("board", &SGA::GameState::board, "Board: a 2 dimensional grid of tiles. This does not contain information about entities on those tiles.")
		.def_readwrite("entities", &SGA::GameState::entities, "List of entities in this game")
		.def_readwrite("players", &SGA::GameState::players, "List of players in this game")
		.def_readwrite("game_info", &SGA::GameState::gameInfo, "Game Info (static information about this game)")
		.def_readwrite("researched_technologies", &SGA::GameState::researchedTechnologies, "Map that indicates if a technology is researched by a player. Key is the player ID and maps to a vector of technology IDs.")

		//Technologies
		.def("is_researched", &SGA::GameState::isResearched, py::arg("playerID"), py::arg("technologyID"), "Check if technology is researched")
		.def("can_research", &SGA::GameState::canResearch, py::arg("playerID"), py::arg("technologyID"), "Check if player can research an technology")
		.def("research_technology", &SGA::GameState::researchTechnology, py::arg("playerID"), py::arg("technologyID"), "Research technology")
		
		//Other
		.def("can_execute_action", py::overload_cast<const SGA::Entity&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Entity can execute a given actionType")
		.def("can_execute_action", py::overload_cast<const SGA::Player&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Player can execute a given actionType")
		.def("get_player_action_types", &SGA::GameState::getPlayerActionTypes, py::arg("playerID"), "Returns a list with all the action types that a player can execute")
		
		////Entities
		.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference,  "Get entity")
		.def("get_entity", py::overload_cast<SGA::Vector2f, float>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity")
		.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity by id")
		.def("get_entity_at", &SGA::GameState::getEntityAt, py::arg("pos"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
		.def("get_entity_const", &SGA::GameState::getEntityConst, py::arg("entityID"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
		.def("add_entity", &SGA::GameState::addEntity, py::arg("type"), py::arg("playerID"), py::arg("position"), "Adds a new entity of a given type to the game, in a given position, belonging to a specific player. ")
		
		////Grid
		.def("is_walkable", &SGA::GameState::isWalkable, py::arg("position"), "Checks if tile is occupied or the tile is walkable")
		.def("is_in_bounds", py::overload_cast<SGA::Vector2f>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")
		.def("is_in_bounds", py::overload_cast<SGA::Vector2i>(&SGA::GameState::isInBounds, py::const_), "Checks if position is inside of the tile map")
		
		////Player
		.def("get_player", py::overload_cast<int>(&SGA::GameState::getPlayer, py::const_),py::return_value_policy::reference, "Get player")
		.def("get_player", py::overload_cast<int>(&SGA::GameState::getPlayer), py::return_value_policy::reference, "Get player")
		.def("get_num_players", &SGA::GameState::getNumPlayers, "Gets the number of players in this game state.")
		
		.def("get_player_entities", py::overload_cast<int>(&SGA::GameState::getPlayerEntities,py::const_), py::return_value_policy::reference, "Gets the list of entities of the specified player.")
		.def("get_player_entities", py::overload_cast<int, SGA::EntityCategory>(&SGA::GameState::getPlayerEntities), py::return_value_policy::reference, "Gets the list of entities of the specified player.")
		.def("get_non_player_entities", py::overload_cast<int, SGA::EntityCategory>(&SGA::GameState::getNonPlayerEntities), py::return_value_policy::reference, "Gets the list of entities that do not belong to the specified player.")
		
		.def("add_player", &SGA::GameState::addPlayer, py::arg("p"), "Adds player to gamestate")
		.def("get_player_parameter", &SGA::GameState::getPlayerParameter, py::arg("playerID"), py::arg("paramName"), "Gets the value of a player parameter. Be sure to check first is the parameter you are asking for exist using SGA::GameState::hasPlayerParameter()")
		.def("has_player_parameter", &SGA::GameState::hasPlayerParameter,py::arg("paramName"), "Indicates if the player has a specific parameter")
		.def("get_player_parameter_names", &SGA::GameState::hasPlayerParameter,py::arg("playerID"), "Returns a list will all the parameter names of the player of which ID is given")
		.def("get_player_parameters", &SGA::GameState::getPlayerParameters,py::arg("playerID"), "Gets a map with all pairs <parameter,value>")
		.def("get_player_score", &SGA::GameState::getPlayerScore,py::arg("playerID"), "Returns the score of the player whose ID is passed.")
		
		.def("apply_fog_of_war", &SGA::GameState::applyFogOfWar, py::arg("playerID"), "Removes entities and hide tiles that are not visible from the point of view of the given player.")
		.def("get_fog_of_war_tile_id", &SGA::GameState::getFogOfWarTileId, "Returns the ID of the tile that represents the fog of war.")
		
		//Print information
		.def("print_state_info", &SGA::GameState::printStateInfo,"Print all the entities of the current state")
		.def("print_board", py::overload_cast<int>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state")
		.def("print_board", py::overload_cast<>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state applying fog")
		.def("print_entity_info", &SGA::GameState::printEntityInfo, py::arg("entityID"), "Print information of a specific entity")
		.def("print_action_info", &SGA::GameState::printActionInfo, py::arg("action"), "Print information of a specific action")
		;

	//---- Random Engine ----
	py::class_<std::mt19937>(m, "mt19937");

	

	// ---- TileType ----
	py::class_<SGA::TileType, std::shared_ptr<SGA::TileType>>(m, "TileType")
		.def(py::init<>())
		.def_readwrite("name", &SGA::TileType::name, "Name of the tile as given in YAML>")
		.def_readwrite("symbol", &SGA::TileType::symbol, "Symbol of this tile as used in YAML for level placement.")
		.def_readwrite("id", &SGA::TileType::id ,"Unique ID for this tile type.")
		.def_readwrite("is_walkable", &SGA::TileType::isWalkable, "Indicates if the tile is walkable by units")
		.def_readwrite("block_sight", &SGA::TileType::blocksSight, "Indicates if this tile blocks the line of sight")
		.def_readwrite("is_default_tile", &SGA::TileType::isDefaultTile, "Indicates if this is a default tile. Default tiles are used for fog of war (to substitute elements that should be hidden) and to be place in positions where units are defined in the YAML level layout.")
		.def("to_tile", &SGA::TileType::toTile, py::arg("x"), py::arg("y"), "Creates an instantiation of this tile given a position");

	// ---- Tile ----
	py::class_<SGA::Tile>(m, "Tile")
		.def(py::init<int, const SGA::TileType*, int, int>(), py::arg("typeID"), py::arg("tileType"), py::arg("x"), py::arg("y"))
		.def_readwrite("is_walkable", &SGA::Tile::isWalkable, "Indicates if this tile is walkable for units to occupy it.")
		.def_readwrite("blocks_sight", &SGA::Tile::blocksSight, "Indicates if this tile blocks the line of sight for entities in the game. ")
		.def_readwrite("position", &SGA::Tile::position, "Position <x,y> of this tile in the board.")
		.def_readwrite("position", &SGA::Tile::position, "Returns the name of this tile type")
		
		.def("get_tile_type_id", &SGA::Tile::getTileTypeID, "Returns the tile type ID of this tile")
		.def("name", &SGA::Tile::name, "Returns the tile name");

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
	py::enum_<SGA::ActionFlag>(m, "ActionFlag", " Used to define how the SGA::ForwardModel::executeAction() will execute this action.")
		.value("None", SGA::ActionFlag::None)
		.value("EndTickAction", SGA::ActionFlag::EndTickAction)
		.value("ContinuousAction", SGA::ActionFlag::ContinuousAction)
		.value("AbortContinuousAction", SGA::ActionFlag::AbortContinuousAction)
		.export_values();
	
	// ---- ActionInfo ----
	py::class_<SGA::ActionInfo>(m, "ActionInfo", "Contains the last tick this action was executed and the action type ID. Used by forward models to check if the SGA::Entity or player can execute the action again after the cooldown.")
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
		.def_readwrite("action_type_flags", &SGA::Action::actionTypeFlags, "Used to define how the SGA::ForwardModel::executeAction() will execute this action.")
		.def_readwrite("targets", &SGA::Action::targets, "Vector with all targets involved in this action. For a UnitAction, index 0 contains the source and index 1 the target of the action. For a PlayerAction, index 0 contains the target of the action. ")
		.def_readwrite("owner_id", &SGA::Action::ownerID, "ID of the owner of this action.")
		.def_readwrite("continuous_action_id", &SGA::Action::continuousActionID, "For continuous action, ID of this action.")
		.def_readwrite("elapsed_ticks", &SGA::Action::elapsedTicks, "Ticks elapsed since this action started.")
		
		.def("execute", &SGA::Action::execute, py::arg("state"), py::arg("fm"), "Execute the effects defined in the SGA::ActionType linked to this action.")
		.def("validate", &SGA::Action::validate, py::arg("state"), "Checks if this action can be executed. It verifies if the conditions defined in the SGA::ActionType linked to this action are passed. This method also checks if the last time of the action execution is higher than the cooldown, and if all the preconditions and target conditions are fullfilled. It also checks that all the actions targets are valid.")
		.def("is_entity_action", &SGA::Action::isEntityAction, "Checks if this action is to be executed over an entity.")
		.def("is_player_action", &SGA::Action::isPlayerAction, "Checks if this action is to be executed over a player.")
		.def("get_source_id", &SGA::Action::getSourceID, "Gets the player or entity ID of the target of this action")
		.def("get_action_type_id", &SGA::Action::getActionTypeID, "Returns the action type ID of this action")
		.def("get_action_name", &SGA::Action::getActionName, "Returns the name of this action")
		.def("get_action_type", &SGA::Action::getActionType, "Checks if this action is to be executed over an entity.")
		.def("get_action_source_type", &SGA::Action::getActionSourceType, "Checks if this action is to be executed over an entity.")

		.def_static("create_end_action", &SGA::Action::createEndAction, py::arg("playerID"), "Generates an Action used by the game to end the tick/turn.")
		.def_static("create_abort_action", py::overload_cast<int,int,int>(&SGA::Action::createAbortAction), "Generates an Action which the owner is a entity, used by the game to abort a continuous action.")
		.def_static("create_abort_action", py::overload_cast<int,int>(&SGA::Action::createAbortAction), "Generates an Action which the owner is a player, used by the game to abort a continuous action.")
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
		.def("is_fullfiled", &SGA:: Condition::isFullfiled, py::arg("state"), py::arg("targets"))
		.def("expr", &SGA:: Condition::expr)
		;
	
	// ---- Effect ----	
	py::class_<SGA::Effect, PyEffect, std::shared_ptr<SGA::Effect>>(m, "Effect")
		.def("execute", &SGA::Effect::execute, py::arg("state"), py::arg("fm"), py::arg("targets"))
		.def("expr", &SGA::Effect::expr)
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
		.def_readwrite("technology_tree_types", &SGA::TechnologyTreeCollection::technologyTreeTypes, "List of technology trees, indexed by a unique ID.")
		.def("get_technology", &SGA::TechnologyTreeCollection::getTechnology, py::arg("technologyID"), "Returns a technology node with an ID corresponding to the one passed by parameter. The node could come from any of the trees of this collection.")
		.def("get_technology_type_id", &SGA::TechnologyTreeCollection::getTechnologyTypeID, py::arg("technologyName"), "Returns the ID of a technology that matches the requested name, in any of the trees of this collection.")
		;
	// ---- TechnologyTreeType ----	
	py::class_<SGA::TechnologyTreeType, std::shared_ptr<SGA::TechnologyTreeType>>(m, "TechnologyTreeType")
		.def_readwrite("technology_tree_name", &SGA::TechnologyTreeType::technologyTreeName, "Name of this technology tree")
		.def_readwrite("technologies", &SGA::TechnologyTreeType::technologies, "Data of this tech tree, each entry contains and ID as key and a technology node as a value")
		.def("get_technology_node", &SGA::TechnologyTreeType::getTechnologyNode, py::arg("technologyID"), "Returns a tech tree node from this tree with a corresponding technology ID.")
		.def("exists_technology_tree_node", &SGA::TechnologyTreeType::existsTechnologyTreeNode, py::arg("technologyID"), "Checks if a given technology ID exists in this tree.")
		.def("getTechnology_node_id", &SGA::TechnologyTreeType::getTechnologyNodeID, py::arg("technologyName"),"Returns the ID of a technology that matches the requested name.")
		.def("exists_technology_node", &SGA::TechnologyTreeType::existsTechnologyNode, py::arg("technologyName"), "Checks if a technology with a given name exists in this tree.")
		;
	// ---- TechnologyTreeNode ----	
	py::class_<SGA::TechnologyTreeNode, std::shared_ptr<SGA::TechnologyTreeNode>>(m, "TechnologyTreeNode")
		.def_readwrite("name", &SGA::TechnologyTreeNode::name,"Name of this technology as given in YAML")
		.def_readwrite("description", &SGA::TechnologyTreeNode::description, "Description of this technology (provided in YAML)")
		.def_readwrite("id", &SGA::TechnologyTreeNode::id, "Unique ID of this technology.")
		.def_readwrite("parent_ids", &SGA::TechnologyTreeNode::parentIDs, "IDs of the technologies that are required for this one to be researchable.")
		.def_readwrite("cost", &SGA::TechnologyTreeNode::cost, "List of pairs (param,cost) that can be spend to research this technology.")
		.def_readwrite("continuousActionTime", &SGA::TechnologyTreeNode::continuousActionTime, "How many ticks/turns takes to research this technology.")
		.def("to_string", &SGA::TechnologyTreeNode::toString,py::arg("gameInfo"), "Returns a string with printable information of this research technology")
		;
	// ---- Grid2D<Tile> ----
	py::class_<SGA::Grid2D<SGA::Tile>>(m, "BoardTiles")
		.def(py::init<int, int, SGA::Tile>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
		.def_readwrite("grid", &SGA::Grid2D<SGA::Tile>::grid)
		.def("get", [](SGA::Grid2D<SGA::Tile>& b, int x, int y) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<SGA::Tile>::getWidth)
		.def("get_height", &SGA::Grid2D<SGA::Tile>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		;
	// ---- Grid2D<TileType> ----
	py::class_<SGA::Grid2D<std::shared_ptr<SGA::TileType>>>(m, "BoardTileTypes")
		.def(py::init<int, int, std::shared_ptr<SGA::TileType>>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
		.def_readwrite("grid", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::grid)
		.def("get", [](const SGA::Grid2D<std::shared_ptr<SGA::TileType>>& b, int x, int y) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getWidth)
		.def("get_height", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		;
	 
	// ---- GameRunner ----
	py::class_<SGA::GameRunner>(m, "GameRunner")
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
		.def("reset", py::overload_cast<int>(&SGA::GameRunner::reset),py::arg("levelID"),"Resets the game to an initial state.")
		.def("reset", py::overload_cast<>(&SGA::GameRunner::reset),"Resets the game to an initial state with a specific map.")
		.def("step", &SGA::GameRunner::step, py::arg("actions"),"Advances the game by one timestep. When the game has ended, you are responsible for calling GameRunner::reset() to reset the environments state.")
		.def("render", &SGA::GameRunner::render,"Renders a visual representation of the game. May create a new window when called for the first time.")
		.def("get_gamestate", &SGA::GameRunner::getGameState, "Returns a reference to the current state of the game.")
		;
	
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

		
		.def_readwrite("technology_tree_collection", &SGA::GameInfo::technologyTreeCollection)
		.def_readwrite("entity_groups", &SGA::GameInfo::entityGroups)

		.def_readwrite("game_description", &SGA::GameInfo::gameDescription)

		.def("get_game_description", &SGA::GameInfo::getGameDescription,"Returns the game description of the game.")
		.def("get_entity_type", &SGA::GameInfo::getEntityType, py::arg("entityTypeID"),"Returns the entity type.")
		.def("get_tile_type", &SGA::GameInfo::getTileType, py::arg("tileTypeID"), "Returns the tile type.")

		.def("get_parameter_global_id", &SGA::GameInfo::getParameterGlobalID, py::arg("parameterName"), "Returns the ID of the parameter.")
		.def("get_parameter_type", &SGA::GameInfo::getParameterType, py::arg("entityTypeID"), py::arg("globalParameterID"), "Returns the parameter type from a entityType.")
		.def("check_entity_have_parameter", &SGA::GameInfo::checkEntityHaveParameter, py::arg("entityTypeID"), py::arg("parameterName"), "Returns true if the entity type has the searched parameter")
		.def("get_player_parameter", &SGA::GameInfo::getPlayerParameter, py::arg("id"), "Returns the parameter of the player.")

		.def("get_action_type_const", &SGA::GameInfo::getActionTypeConst, py::arg("actionTypeID"), "Returns the action type.")
		.def("get_action_type", &SGA::GameInfo::getActionType, py::arg("typeID"), "Returns the action type.")
		.def("get_action_type_id", &SGA::GameInfo::getActionTypeID, py::arg("actionName"),"Returns the action type ID.")

		.def("get_technology_counts", &SGA::GameInfo::getTechnologyCounts, "Returns a map of tuples with the tree ID and how many technologies each has.")
		.def("get_technology", &SGA::GameInfo::getTechnology, py::arg("technologyID"), "Returns the action type.")
		.def("get_technology_type_id", &SGA::GameInfo::getTechnologyTypeID, py::arg("technologyName"), "Returns the ID of a technology that matches the requested name, in any of the trees of the tree collection.")
		.def("get_tree_nodes", &SGA::GameInfo::getTreeNodes, py::arg("techTreeID"), "Returns a list with all technologies of a given tree, specified by its ID")
		.def("get_tech_trees_ids", &SGA::GameInfo::getTechTreesIDs, "Returns the IDs of the tech trees of this game.")

		.def("get_entity_types",
			[](SGA::GameInfo& a)
			{
				return a.entityTypes.get();
			}
		)
		.def("get_action_types",
			[](SGA::GameInfo& a)
			{
				return a.actionTypes.get();
			}
		)
		.def("get_tile_types",
			[](SGA::GameInfo& a)
			{
				return a.tileTypes.get();
			}
		)
		;

	// ---- GameObserver ----
	py::class_<SGA::GameObserver>(m, "GameObserver");


	// ---- Heuristic ----
	py::class_<SGA::MinimizeDistanceHeuristic>(m, "MinimizeDistanceHeuristic")
		.def(py::init<>())
		.def("get_name", &SGA::MinimizeDistanceHeuristic::getName)
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
		.def("assign_action_or_replace", &SGA::ActionAssignment::assignActionOrReplace, py::arg("newAction"), "Assigns the action to the corresponding entity or player. Any existing assignment will be overwritten.")
		.def("merge", &SGA::ActionAssignment::merge, py::arg("merge"), "Copies the given action-assignment into this object. Any existing assignment will be overwritten in case of a conflict.")
		.def("clear", &SGA::ActionAssignment::clear, "Deletes all assignments.")
		.def("get_entity_action", &SGA::ActionAssignment::getEntityAction, py::arg("entityID")," Returns the assignment for an entity.")
		.def("get_player_action", &SGA::ActionAssignment::getPlayerAction, py::arg("playerID"), "Returns the assignment for an player.")
		.def("get_assignment_count", &SGA::ActionAssignment::getAssignmentCount, "Returns the amount of actions that are assigned to entities and players.")
		.def("get_entity_actions", &SGA::ActionAssignment::getEntityActions)
		.def("get_player_actions", &SGA::ActionAssignment::getPlayerActions)
		.def_static("from_single_action", &SGA::ActionAssignment::fromSingleAction, py::arg("a"), "Constructs a ActionAssignment from a single action, useful for TBS-Games.")
		.def_static("create_end_action_assignment", &SGA::ActionAssignment::createEndActionAssignment, py::arg("playerID"), "Generates an ActionAssignment used by the game to end the tick/turn.");

	// ---- OnTickEffect ----
	py::class_<SGA::OnTickEffect>(m, "OnTickEffect")
		.def(py::init<>())
		.def_readwrite("valid_targets", &SGA::OnTickEffect::validTargets)
		.def_readwrite("conditions", &SGA::OnTickEffect::conditions)
		.def_readwrite("effects", &SGA::OnTickEffect::effects)
		;
	// ---- OnEntitySpawnEffect ----
	py::class_<SGA::OnEntitySpawnEffect>(m, "OnEntitySpawnEffect")
		.def(py::init<>())
		.def_readwrite("valid_targets", &SGA::OnEntitySpawnEffect::validTargets)
		.def_readwrite("conditions", &SGA::OnEntitySpawnEffect::conditions)
		.def_readwrite("effects", &SGA::OnEntitySpawnEffect::effects)
		;
		
	// ---- Forward model ----
	py::class_<SGA::ForwardModel>(m, "EntityForwardModel")

		.def_readwrite("on_tick_effects", &SGA::ForwardModel::onTickEffects)
		.def_readwrite("on_entity_spawn_effects", &SGA::ForwardModel::onEntitySpawnEffects)
		.def_readwrite("win_conditions", &SGA::ForwardModel::winConditions)
		.def_readwrite("lose_conditions", &SGA::ForwardModel::loseConditions)
		//.def_readwrite("action_targets", &SGA::ForwardModel::actionTargets)

		.def("can_player_lost", &SGA::ForwardModel::checkPlayerLost, py::arg("state"), py::arg("player"),"Checks if a player has lost the game due to the game's lose conditions.")
		.def("check_player_won", &SGA::ForwardModel::checkPlayerWon, py::arg("state"), py::arg("player"),"Returns if a player won the game by attending to the winning conditions defined in the rules.")
		.def("spawn_entity", &SGA::ForwardModel::spawnEntity, py::arg("state"), py::arg("entityType"), py::arg("playerID"), py::arg("position"),"Spawns an entity in the game with the default spawn entity method. A list of On Spawn effects are  executed just after spawning the entity")
		;
		
		
	py::class_<SGA::TBSForwardModel, SGA::ForwardModel>(m, "TBSForwardModel")
		.def(py::init<>())
		.def("generate_actions", py::overload_cast<const SGA::GameState& , int>(&SGA::ForwardModel::generateActions, py::const_)," Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.")
		.def("generate_actions", py::overload_cast<const SGA::GameState& , int, std::vector<SGA::Action>&>(&SGA::ForwardModel::generateActions, py::const_), "Returns a list of available actions in the given gamestate by the received player")

		.def("advance_gamestate", py::overload_cast<SGA::GameState& , const SGA::Action&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes an action in a given SGA::GameState before updating the entities of the gamestate that should be removed and checking if the game is over.")
		.def("advance_gamestate", py::overload_cast<SGA::GameState& , const SGA::ActionAssignment&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes a list of actions.")

		.def("end_turn", &SGA::TBSForwardModel::endTurn, py::arg("state"),"End the turn of the current player and if all the player has played it ends the current game turn.")
		//.def("is_valid", &SGA::TBSForwardModel::isValid, py::arg("state"), py::arg("action"),"Check if an action is valid in a given state")
		.def("check_game_is_finished", &SGA::TBSForwardModel::checkGameIsFinished, py::arg("state")," Checks if the game is finished by current limit or because a player has won.")
		;

	py::class_<SGA::RTSForwardModel, SGA::ForwardModel>(m, "RTSForwardModel")
		.def(py::init<>())
		.def_readwrite("delta_time", &SGA::RTSForwardModel::deltaTime)

		.def("generate_actions", py::overload_cast<const SGA::GameState&, int>(&SGA::ForwardModel::generateActions, py::const_), " Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.")
		.def("generate_actions", py::overload_cast<const SGA::GameState&, int, std::vector<SGA::Action>&>(&SGA::ForwardModel::generateActions, py::const_), "Returns a list of available actions in the given gamestate by the received player")

		.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::Action&>(&SGA::RTSForwardModel::advanceGameState, py::const_),"Moves all the entities and resolves collisions before and after executing an action in a given Gamestate")
		.def("advance_gamestate", py::overload_cast<SGA::GameState&, const SGA::ActionAssignment&>(&SGA::RTSForwardModel::advanceGameState, py::const_),"Moves all the entities and resolves collisions before and after executing an action in a given Gamestate")

		.def("move_entities", &SGA::RTSForwardModel::moveEntities, py::arg("state"), "Moves all the entities that have a current path and they did not reach their destination. If the entity has a path it moves the entity through all the path points one after theother until reaching the last one.")
		.def("resolve_entity_collisions", &SGA::RTSForwardModel::resolveEntityCollisions, py::arg("state"),"Resolves collisions between entities in a basic way computing the penetration depth and pushing them way in the opposite direction.")
		.def("resolve_environment_collisions", &SGA::RTSForwardModel::resolveEnvironmentCollisions, py::arg("state"),"Resolves collisions between entities and the tiles that are not walkable in a basic way computing the penetration depth and pushing them way in the opposite direction.")

		.def("find_path", &SGA::RTSForwardModel::findPath, py::arg("state"), py::arg("startPos"), py::arg("endPos"),"Returns a Path inside the Navmesh between the start and end positons.")
		.def("check_game_is_finished", &SGA::RTSForwardModel::checkGameIsFinished, py::arg("state"),"Checks if the game is finished by current limit or because a player has won.")
		;

	// ---- Path ----
	py::class_<SGA::Path>(m, "Path")
		.def(py::init<>())
		//.def_readwrite("m_straight_path", &SGA::Path::m_straightPath)
		.def_readwrite("m_nstraight_path", &SGA::Path::m_nstraightPath)
		//.def_readwrite("m_straight_path_flags", &SGA::Path::m_straightPathFlags)
		.def_readwrite("m_straight_path_options", &SGA::Path::m_straightPathOptions)
		.def_readwrite("current_path_index", &SGA::Path::currentPathIndex)
		.def("is_empty", &SGA::Path::isEmpty);


	// ---- Agent ----
	py::class_<SGA::Agent, PyAgent, std::shared_ptr<SGA::Agent>/* <--- trampoline*/>(m, "Agent")
		.def(py::init<std::string>(), py::arg("name")="PythonAgent")
		.def_readwrite("agent_name",&SGA::Agent::agentName)
		.def("computeAction", &SGA::Agent::computeAction, "Function for deciding the next action to execute. Must be overriden for an agent to work. Returns an ActionAssignment")
		.def("init", &SGA::Agent::init, "Function for initializing the agent. Override this function to receive a call just before starts.")
		.def("get_player_id", &SGA::Agent::getPlayerID);

	// ---- Arena ----
	py::class_<Arena>(m, "Arena")
		.def("run_games", py::overload_cast<int,int,int,int>(&Arena::runGames))
		.def("run_games",
			[](Arena& a, int playerCount, int seed, int gamesNumber, int mapNumber, py::list agents)
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

				py::gil_scoped_release release;
				a.runGames(playerCount, seed, gamesNumber, mapNumber, newAgents);
				py::gil_scoped_acquire acquire;
			}
		)		
		;

	// ---- Timer ----
	py::class_<SGA::Timer>(m, "Timer")
		.def(py::init<long>(), py::arg("maxTimeMs")=0)
		.def("start", &SGA::Timer::start)
		.def("stop", &SGA::Timer::stop)
		.def("elapsed_milliseconds", &SGA::Timer::elapsedMilliseconds)
		.def("elapsed_seconds", &SGA::Timer::elapsedSeconds)
		.def("elapsed_minutes", &SGA::Timer::elapsedMinutes)
		.def("elapsed_hours", &SGA::Timer::elapsedHours)
		.def("set_max_time_milliseconds", &SGA::Timer::setMaxTimeMilliseconds, py::arg("newMaxTimeMs"))
		.def("remaining_time_milliseconds", &SGA::Timer::remainingTimeMilliseconds)
		.def("exceeded_max_time", &SGA::Timer::exceededMaxTime)
		.def("get_max_time_milliseconds", &SGA::Timer::getMaxTimeMilliseconds)
		;

}