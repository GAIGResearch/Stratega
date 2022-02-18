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
//#include <filesystem>
#include <Stratega/Logging/Log.h>
#include <limits>
#include <Stratega/Utils/Timer.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max
//#include <Stratega/Utils/>
namespace py = pybind11;

// STL
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Action>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Player>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity*>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Agent*>);
PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SGA::Agent>>);


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

std::string getModulePath()
{
	py::object stratega = py::module::import("stratega");
	return stratega.attr("__file__").cast<std::string>();
}

bool isLocalResourcesPath(std::string& modulePath, std::string& configPath)
{
	//Check if config exist	
	ghc::filesystem::path newPossiblePath(modulePath);
	newPossiblePath=newPossiblePath.parent_path() / configPath;
	
	if (ghc::filesystem::exists(newPossiblePath))
		return true;		
	else	
		return false;	
}

 std::unique_ptr<SGA::GameConfig> loadConfig(std::string& path)
 {
	 auto modulePath = getModulePath();
	 ghc::filesystem::path modulePythonPath(modulePath);	

	 modulePythonPath.make_preferred();
	 ghc::filesystem::path pathCheck(path);

	 if(pathCheck.is_absolute())
	 {
		 auto resourcePath = (modulePythonPath.parent_path() / "resources").string();		 
		 auto config = SGA::loadConfigFromYAML(path, (modulePythonPath.parent_path() / "resources").string());
		 return std::move(config);
	 }
	 else
	 { 
		 if (isLocalResourcesPath(modulePath,path))
		 {
			 auto newPossiblePath = modulePythonPath.parent_path() / path;
			 auto config = SGA::loadConfigFromYAML(newPossiblePath.string(), (modulePythonPath.parent_path() / "resources").string());
			 return std::move(config);
		 }
		 else
		 {
			 auto config = SGA::loadConfigFromYAML(path, (modulePythonPath.parent_path() / "resources").string());			 
			 return std::move(config);
		 }
	 }	 
 }

 std::unique_ptr<SGA::GameRunner> createRunner(const SGA::GameConfig* gameConfig)
 {
	 return std::move(SGA::createGameRunner(*gameConfig));
 }

 std::vector<std::shared_ptr<SGA::Agent>> generateAgents(const SGA::GameConfig* gameConfig)
 {
	 auto agents = gameConfig->generateAgents();
	 std::vector<std::shared_ptr<SGA::Agent>> newAgents;

	 std::move(agents.begin(), agents.end(), std::back_inserter(newAgents));

	 return newAgents;
 }

 void setDefaultLogger(std::string logPath)
 {
	 SGA::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
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

	m.doc() = R"pbdoc(
        Stratega python bindings
        -----------------------
        .. currentmodule:: stratega
        .. autosummary::
           :toctree: _generate

			load_config
			create_runner
			create_arena
			generate_agents
			set_default_logger
			load_levels_from_yaml
			ActionType
			EntityType
			Player
			Entity
			GameType
			GameDescription
			EntityCategory
			ActionCategory
			GameState
			TileType
			Tile
			Parameter
			ActionFlag
			ActionInfo
			Action
			ActionSourceType
			TargetTypeEnum
			TargetType
			SamplingMethod
			Condition
			Effect
			ActionTargetEnum
			ActionTarget
			EntityPlacement
			LevelDefinition
			TechnologyTreeCollection
			TechnologyTreeType
			TechnologyTreeNode
			BoardTiles
			GameRunner
			GameConfig
			GameInfo
			GameObserver
			MinimizeDistanceHeuristic
			ActionAssignment
			OnTickEffect
			OnEntitySpawnEffect
			EntityForwardModel
			TBSForwardModel
			RTSForwardModel
			Path
			Agent
			Arena
			Timer
    )pbdoc";

	m.def("load_config", &loadConfig, "Loads game config", py::arg("path"),py::call_guard<py::scoped_ostream_redirect,
		py::scoped_estream_redirect>());
	m.def("create_runner", &createRunner, "Create game runner", py::arg("gameConfig"));
	m.def("create_arena", &createArena, "Create game aren", py::arg("gameConfig"));
	m.def("generate_agents", &generateAgents, "Generate agents", py::arg("gameConfig"));
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

	//Buffs
	py::bind_vector<std::vector<SGA::Buff>>(m, "BuffsList");
	py::bind_vector<std::vector<SGA::BuffType>>(m, "BuffsTypesList");
	py::bind_map<std::unordered_map<int, SGA::BuffType>>(m, "BuffTypesMap");
	
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
		.def("get_target_conditions", &SGA::ActionType::getTargetConditions, py::arg("searchingTarget"), "Returns a list of conditions linked to the searched target.")
		
		.def_property("name", &SGA::ActionType::getName, &SGA::ActionType::setName)
		.def("get_name", &SGA::ActionType::getName, "Name of this action type.")
		.def("set_name", &SGA::ActionType::setName, py::arg("name"), "Name of this action type.")

		.def_property("id", &SGA::ActionType::getID, &SGA::ActionType::setID)
		.def("get_id", &SGA::ActionType::getID, "Unique ID of this action type")
		.def("set_id", &SGA::ActionType::setID, py::arg("id"), "Unique ID of this action type")
		
		.def_property("source_type", &SGA::ActionType::getSourceType, &SGA::ActionType::setSourceType)
		.def("get_source_type", &SGA::ActionType::getSourceType, "Source of this action (entity, player...)")
		.def("set_source_type", &SGA::ActionType::setSourceType, py::arg("ast"), "Source of this action (entity, player...)")

		.def_property("cooldown", &SGA::ActionType::getCooldown, &SGA::ActionType::setCooldown)
		.def("get_cooldown", &SGA::ActionType::getCooldown, "Number of ticks/turn that must happen between two uses of this action.")
		.def("set_Cooldown", &SGA::ActionType::setCooldown, py::arg("cd"), "Number of ticks/turn that must happen between two uses of this action.")

		.def_property("continuous", &SGA::ActionType::isContinuous, &SGA::ActionType::setContinuous)
		.def("is_continuous", &SGA::ActionType::isContinuous)
		.def("set_continuous", &SGA::ActionType::setContinuous, py::arg("c"))

		.def("get_preconditions",
			[](SGA::ActionType& actionType)
			{
				return actionType.getPreconditions();
			}
			, "List of preconditions for this action to be run. These are tier 1 conditions: if they can't be fulfilled the action is not possible. If the conditions are true, the action may be possible based on situational circumstances.  (Example: having a resource to create an entity but no space in the board to spawn it)."
			)
		.def("get_targets",
			[](SGA::ActionType& actionType)
			{
				return actionType.getTargets();
			}
			, "List of target types of this action with their conditions. Each target is a pair <target, list<conditions>>. These are tier 2 conditions (i.e. situational), to be checked during action generation."
			)
		.def("get_trigger_complete",
			[](SGA::ActionType& actionType)
			{
				return actionType.getTriggerComplete();
			}
			,"Continuous actions finish when certain conditions are met. This vector indicates those conditions."
			)
		.def("get_effects",
			[](SGA::ActionType& actionType)
			{
				return actionType.getEffects();
			}
			," List of effects this action causes when executed. This list is populated only for non continuous actions."
			)
		.def("get_on_start",
			[](SGA::ActionType& actionType)
			{
				return actionType.getOnStart();
			}
			,"For continuous actions, list of effects that are applied when this action starts."
			)
		.def("get_on_tick",
			[](SGA::ActionType& actionType)
			{
				return actionType.getOnTick();
			}
			,"For continuous actions, list of effects that are applied on every tick/turn. This effect is *not* applied on the tick this action starts, but it *will* also be applied in the tick it ends."
			)
		.def("get_on_complete",
			[](SGA::ActionType& actionType)
			{
				return actionType.getOnComplete();
			}
			,"For continuous actions, list of effects that are applied when this action ends."
			)
		.def("get_on_abort",
			[](SGA::ActionType& actionType)
			{
				return actionType.getOnAbort();
			}
			,"For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in the game tick they have been aborted."
			)
		;

	// ---- Buff type ----
	py::class_<SGA::BuffType>(m, "BuffType", "Contains the definition of an Buff.")
		.def(py::init<>())

		.def_property("name", &SGA::BuffType::getName, &SGA::BuffType::setName)
		.def("get_name", &SGA::BuffType::getName, "Name of this buff type.")
		.def("set_name", &SGA::BuffType::setName, py::arg("name"), "Name of this buff type.")

		.def_property("id", &SGA::BuffType::getID, &SGA::BuffType::setID)
		.def("get_id", &SGA::BuffType::getID, "Unique ID of this buff type")
		.def("set_id", &SGA::BuffType::setID, py::arg("id"), "Unique ID of this buff type")
		;
	

	// ---- Buff ----
	py::class_<SGA::Buff>(m, "Buff", "Contains the information of an Buff.")
		.def(py::init<>())

		.def("get_buff_type", &SGA::Buff::getType, "Return buff type")

		.def("get_elapsed_ticks", &SGA::Buff::getElapsedTicks, "Get the elapsed ticks")
		.def("set_elapsed_ticks", &SGA::Buff::setElapsedTicks, py::arg("newElapsedTicks"), "Set elapsed ticks")

		.def("get_duration_ticks", &SGA::Buff::getDurationTicks, "Get the duration ticks")
		.def("set_duration_ticks", &SGA::Buff::setDurationTicks, py::arg("newDurartionTicks"), "Set duration ticks")
		;

	// ---- Entity type ----
	py::class_<SGA::EntityType, std::shared_ptr<SGA::EntityType>>(m, "EntityType", "Contains the definition of the entity type. This information includes a list of parameters definitions, the actions that can execute, required technologies to spawn a entity, a list of spawnable entity types and a list of cost assigned to this entity type.")
		.def(py::init<>())
		
		.def("get_parameter", &SGA::EntityType::getParameter, py::arg("id"), "Returns a SGA::Parameter reference that contains the parameter defintion")		
		.def("can_execute_action", &SGA::EntityType::canExecuteAction, py::arg("actionTypeID"), "Checks if this entity type is allowed to execute an action type")
		.def("instantiate_entity", &SGA::EntityType::instantiateEntity, py::arg("entityID"), "Generate a new empty instance of this entity type ")
		.def("get_param_max", &SGA::EntityType::getParamMax, py::arg("paramName"), "Returns the maximum value of a given parameter")
		.def("get_param_min", &SGA::EntityType::getParamMin, py::arg("paramName"), " Returns the minimum value of a given parameter")

		.def_property("id", &SGA::EntityType::getID, &SGA::EntityType::setID)
		.def("get_id", &SGA::EntityType::getID, "Unique ID of this entity type")
		.def("set_id", &SGA::EntityType::setID, py::arg("id"), "Unique ID of this entity type")

		.def_property("name", &SGA::EntityType::getName, &SGA::EntityType::setName)
		.def("get_name", &SGA::EntityType::getName, "Name of this type, as defined in the YAML file.")
		.def("set_name", &SGA::EntityType::setName, py::arg("name"), "Name of this type, as defined in the YAML file.")

		.def_property("symbol", &SGA::EntityType::getSymbol, &SGA::EntityType::setSymbol)
		.def("get_symbol", &SGA::EntityType::getSymbol, "Symbol for this entity used in YAML for level placement.")
		.def("set_symbol", &SGA::EntityType::setSymbol, py::arg("s"), "Symbol for this entity used in YAML for level placement.")
		
		.def_property("required_tech_id", &SGA::EntityType::getRequiredTechID, &SGA::EntityType::setRequiredTechID)
		.def("get_required_tech_id", &SGA::EntityType::getRequiredTechID, "ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.")
		.def("set_required_tech_id", &SGA::EntityType::setRequiredTechID, py::arg("id"), "ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.")
		
		.def_property("continuous_action_time", &SGA::EntityType::getContinuousActionTime, &SGA::EntityType::setContinuousActionTime)
		.def("get_continuous_action_time", &SGA::EntityType::getContinuousActionTime, "Continuous Action Time")
		.def("set_continuous_action_time", &SGA::EntityType::setContinuousActionTime, py::arg("d"), "Continuous Action Time")
		
		.def_property("line_of_sight_range", &SGA::EntityType::getLoSRange, &SGA::EntityType::setLoSRange)
		.def("get_line_of_sight_range", &SGA::EntityType::getLoSRange, "Range of the line of sight of this unit.")
		.def("set_line_of_sight_range", &SGA::EntityType::setLoSRange, py::arg("d"), "Range of the line of sight of this unit.")
		
		.def("get_parameters",
			[](SGA::EntityType& entityType)
			{
				return entityType.getParameters();
			}
			, "Map of parameters of this entity. Key is an integer (ID of the parameter) and value is the paramter object. The parameter defines the min, max and default value this parameter can take."
				)

		.def("get_action_ids",
			[](SGA::EntityType& entityType)
			{
				return entityType.getActionIDs();
			}
			, "List of action IDs this entity can execute."
				)
		.def("get_spawnable_entity_types",
			[](SGA::EntityType& entityType)
			{
				return entityType.getSpawnableEntityTypes();
			}
			, "For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in the game tick they have been aborted."
				)
		.def("get_costs",
			[](SGA::EntityType& entityType)
			{
				return entityType.getCosts();
			}
			, "List of parameters and values that must be spent to spawn/build/create this unit."
				)

		;

	// ---- Player ----
	py::class_<SGA::Player>(m, "Player")
		.def("__copy__", [](const SGA::Player& self) {
		return SGA::Player(self);
			})
		.def("__deepcopy__", [](const SGA::Player& self, py::dict) {
				return SGA::Player(self);
			})

		.def(py::init<int, bool>(), py::arg("id"), py::arg("canPlay"))

		.def("can_execute_action", &SGA::Player::canExecuteAction, py::arg("actionTypeID"), "Indicates if a given action type can be executed by this player.")
		.def("get_action_info", &SGA::Player::getActionInfo, py::arg("actionTypeID"), " Returns the ActionInfo of an action type this player can execute.")

		.def("get_neutral_player_id", &SGA::Player::getNeutralPlayerID, "Returns the player ID defined for a neutral player.")
		.def("get_id", &SGA::Player::getID, "Returns the ID of this player")
		.def("can_play", &SGA::Player::canPlay, "Indicates if this player can play at this time.")
		.def("set_can_play", &SGA::Player::setCanPlay, py::arg("can"), "Sets if the player can play now or not.")
		.def("get_parameters",
			[](SGA::Player& player)
			{
				return player.getParameters();
			}
			, "Returns the list of parameters, can't be modified."
				)
		.def("get_parameter",
			[](SGA::Player& player, int paramIdx)
			{
				return player.getRawParameterAt(paramIdx);
			}
			, "Returns a const value of a parameter of this player."
				)


		.def("set_parameter", &SGA::Player::setParameter, py::arg("paramIdx"), py::arg("val"), " Sets the parameter of this player to a certain value")
		.def("resize_parameters", &SGA::Player::resizeParameters, py::arg("cap"), " Sets a size for the vector of parameters of this player.")
		.def("remove_continuous_action", &SGA::Player::removeContinuousAction, py::arg("idx"), " Sets a size for the vector of parameters of this player.")
		.def("add_continuous_action", &SGA::Player::addContinuousAction, py::arg("newAction"), " Adds a continuous action to the list of this player.")
		.def("advance_continuous_action", &SGA::Player::advanceContinuousAction, py::arg("idx"), "Advances the tick counter of the continuous action with index 'idx'")
		.def("get_continuous_actions", &SGA::Player::getContinuousActions, "Returns the list of continuous actions this player is executing.")

		.def("get_attached_actions", &SGA::Player::getAttachedActions, " Returns the list of attached actions to this player.")
		.def("get_attached_action", &SGA::Player::getAttachedAction, py::arg("idx"), " Returns the action info of the attached action with index 'idx'.")

		.def("add_attached_action", &SGA::Player::addAttachedAction, py::arg("actionTypeID"),py::arg("lastExecutedTick"), "Adds a new attached action to this player.")
		.def("set_action_last_tick", &SGA::Player::setActionLastTick, py::arg("idx"),py::arg("lastTick"), "Sets the last tick on an attached action, indexed by 'idx'")
		
		.def("res_attached_actions", &SGA::Player::resAttachedActions, py::arg("cap"), "Reserves space for attached actions.")
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

		.def("get_entity_type", &SGA::Entity::getEntityType, "Return entity type")
		.def("get_entity_type_id", &SGA::Entity::getEntityTypeID, "Return entity type id")
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
		.def("get_buffs", py::overload_cast<>(&SGA::Entity::getBuffs, py::const_), "Gets the list of buffs. Modifiable.")
		.def("get_buffs", py::overload_cast<>(&SGA::Entity::getBuffs), "Gets the list of buffs.")
		.def("get_entity_parameter_names", &SGA::Entity::getEntityParameterNames, "Gets the list of parameters names.")
		.def("get_entity_parameters", &SGA::Entity::getEntityParameters, " Gets a map with all pairs <parameter,value>")
		.def("get_parameter", &SGA::Entity::getParameter, py::arg("paramName"), "Gets the value of a specific parameter, by name ")
		.def("get_parameter_at", &SGA::Entity::getParameterAt, py::arg("paramIdx"), "Gets a specific parameters, by index ")

		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions, py::const_), "Gets the list of continuous actions attached to this entity. Modifiable.")
		.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions), "Gets the list of continuous actions attached to this entity. ")
		
		.def("get_movement_speed", &SGA::Entity::getMovementSpeed, "Returns the movement speed of this entity.")
		.def("get_collision_radius", &SGA::Entity::getCollisionRadius, " Returns the collision radius of this entity.")
		.def("get_id", &SGA::Entity::getID, "Returns theID of this entity.")
		.def("get_owner_id", &SGA::Entity::getOwnerID, "Returns the owner ID (i.e. player ID) who controls this entity.")
		.def("set_owner_id", &SGA::Entity::setOwnerID, py::arg("oID"), "Returns the owner ID (i.e. player ID) who controls this entity.")
		.def("get_line_of_sight_range", &SGA::Entity::getLineOfSightRange,  "Returns the line of sight of this entity.")
		.def("x", &SGA::Entity::x,  "Returns y position of this entity.")
		.def("y", &SGA::Entity::y,  "Returns y position of this entity.")
		.def("get_position", &SGA::Entity::getPosition,  "Returns y position of this entity.")
		.def("set_position", &SGA::Entity::setPosition, py::arg("v"), "Sets the position of this entity in the board. Does not modify the board.")
		.def("get_path", &SGA::Entity::getPath, "Returns the path that this entity is following (RTS games only) ")
		.def("inc_path_index", &SGA::Entity::incPathIndex, "Increments the current index of the path that this entity is following (RTS games only) ")
		.def("set_path", &SGA::Entity::setPath, "Sets the path this entity is following (RTS games only) ")
		;

	// ---- GameType ----
	py::enum_<SGA::GameType>(m, "GameType")
		.value("TBS", SGA::GameType::TBS)
		.value("RTS", SGA::GameType::RTS)
		.export_values();

	//---- GameDescription ----
	py::class_<SGA::GameDescription,std::shared_ptr<SGA::GameDescription>>(m, "GameDescription")
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
		.def("can_execute_action", py::overload_cast<const SGA::Entity&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Entity can execute a given actionType")
		.def("can_execute_action", py::overload_cast<const SGA::Player&, const SGA::ActionType& >(&SGA::GameState::canExecuteAction, py::const_), "Checks if a Player can execute a given actionType")
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

		.def("initBoard", &SGA::GameState::initBoard, py::arg("boardWidth"), py::arg("tiles"),"Initializes the board with the tiles passed by parameter. ")

		.def("get_board_height", &SGA::GameState::getBoardHeight,"Returns the height of the board.")
		.def("get_board_width", &SGA::GameState::getBoardWidth,"Returns the width of the board.")

		.def("get_tile_at", py::overload_cast<int, int>(&SGA::GameState::getTileAt, py::const_), "Returns the tile at the position indicated in the parameter. Can throw an exception if out of bounds.")
		.def("get_tile_at", py::overload_cast<const SGA::Vector2i&>(&SGA::GameState::getTileAt, py::const_), "Returns the tile at the position (x,y) indicated in the parameter. Can throw an exception if out of bounds.")
		
		////Entities
		.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity")
		.def("get_entity", py::overload_cast<SGA::Vector2f, float>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity")
		.def("get_entity", py::overload_cast<int>(&SGA::GameState::getEntity), py::return_value_policy::reference, "Get entity by id")
		.def("get_entity_at", &SGA::GameState::getEntityAt, py::arg("pos"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
		.def("get_entity_const", &SGA::GameState::getEntityConst, py::arg("entityID"), py::return_value_policy::reference, "Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. ")
		.def("add_entity", &SGA::GameState::addEntity, py::arg("type"), py::arg("playerID"), py::arg("position"), "Adds a new entity of a given type to the game, in a given position, belonging to a specific player. ")
		
		.def("get_player_entities", &SGA::GameState::getPlayerEntities, py::arg("playerID"), py::arg("entityCategory")=SGA::EntityCategory::Null, "Gets the list of entities of the specified player.")
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
		.def("has_player_parameter", &SGA::GameState::hasPlayerParameter,py::arg("paramName"), "Indicates if the player has a specific parameter")
		.def("get_player_parameter_names", &SGA::GameState::hasPlayerParameter,py::arg("playerID"), "Returns a list will all the parameter names of the player of which ID is given")
		.def("get_player_parameters", &SGA::GameState::getPlayerParameters,py::arg("playerID"), "Gets a map with all pairs <parameter,value>")
		.def("get_player_score", &SGA::GameState::getPlayerScore,py::arg("playerID"), "Returns the score of the player whose ID is passed.")
		
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
		.def("print_state_info", &SGA::GameState::printStateInfo,"Print all the entities of the current state")
		.def("print_board", py::overload_cast<int>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state")
		.def("print_board", py::overload_cast<>(&SGA::GameState::printBoard, py::const_), "Print view of the map of the current state applying fog")
		.def("print_entity_info", &SGA::GameState::printEntityInfo, py::arg("entityID"), "Print information of a specific entity")
		.def("print_action_info", &SGA::GameState::printActionInfo, py::arg("action"), "Print information of a specific action")
		;

	//---- Random Engine ----
	py::class_<boost::mt19937>(m, "mt19937");

	

	// ---- TileType ----
	py::class_<SGA::TileType, std::shared_ptr<SGA::TileType>>(m, "TileType")
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
	py::class_<SGA::Tile>(m, "Tile")
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

	// ---- Parameter ----
	py::class_<SGA::Parameter>(m, "Parameter")
		.def(py::init<>())

		.def_property("id", &SGA::Parameter::getID, &SGA::Parameter::setID)
		.def("set_id", &SGA::Parameter::setID, py::arg("id"))
		.def("get_id", &SGA::Parameter::getID)

		.def_property("name", &SGA::Parameter::getName, &SGA::Parameter::setName)
		.def("get_name", &SGA::Parameter::getName)
		.def("set_name", &SGA::Parameter::setName, py::arg("name"))

		.def_property("index", &SGA::Parameter::getIndex, &SGA::Parameter::setIndex)
		.def("get_index", &SGA::Parameter::getIndex)
		.def("set_index", &SGA::Parameter::setIndex, py::arg("idx"))

		.def_property("default_value", &SGA::Parameter::getDefaultValue, &SGA::Parameter::setDefaultValue)
		.def("get_default_value", &SGA::Parameter::getDefaultValue)
		.def("set_default_value", &SGA::Parameter::setDefaultValue, py::arg("val"))

		.def_property("min_value", &SGA::Parameter::getMinValue, &SGA::Parameter::setMinValue)
		.def("get_min_value", &SGA::Parameter::getMinValue)
		.def("set_min_value", &SGA::Parameter::setMinValue, py::arg("val"))

		.def_property("max_value", &SGA::Parameter::getMaxValue, &SGA::Parameter::setMaxValue)
		.def("get_max_value", &SGA::Parameter::getMaxValue)
		.def("set_max_value", &SGA::Parameter::setMaxValue, py::arg("val"))
		;

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
		.def_static("create_abort_entity_action", &SGA::Action::createAbortEntityAction, "Generates an Action which the owner is a entity, used by the game to abort a continuous action.")
		.def_static("create_abort_player_action", &SGA::Action::createAbortPlayerAction, "Generates an Action which the owner is a player, used by the game to abort a continuous action.")
		
		.def_property("max_value", &SGA::Action::getOwnerID, &SGA::Action::setOwnerID)
		.def("get_owner_id", &SGA::Action::getOwnerID)
		.def("set_owner_id", &SGA::Action::setOwnerID, py::arg("id"))

		.def_property("continuous_action_id", &SGA::Action::getContinuousActionID, &SGA::Action::setContinuousActionID)
		.def("get_continuous_action_id", &SGA::Action::getContinuousActionID)
		.def("set_continuous_action_id", &SGA::Action::setContinuousActionID, py::arg("id"))

		.def_property("elapsed_ticks", &SGA::Action::getElapsedTicks, &SGA::Action::setElapsedTicks)
		.def("get_elapsed_ticks", &SGA::Action::getElapsedTicks)
		.def("incTicks", &SGA::Action::incTicks)
		.def("set_elapsed_ticks", &SGA::Action::setElapsedTicks, py::arg("elapsed"))

		.def_property("action_flag", &SGA::Action::getActionFlag, &SGA::Action::setActionFlag)
		.def("get_action_flag", &SGA::Action::getActionFlag)
		.def("set_action_flag", &SGA::Action::setActionFlag, py::arg("flag"))

		.def("get_targets",
			[](SGA::Action& action)
			{
				return action.getTargets();
			}
		)
		.def("set_action_targets", &SGA::Action::setActionTargets, py::arg("actionTargets"))

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

		.def("get_type_string", &SGA::TargetType::getTypeString)		
		.def("is_valid", &SGA::TargetType::isValid, py::arg("state"), py::arg("actionTarget"), py::arg("sourceActionTarget"))

		.def_property("type", &SGA::TargetType::getType, &SGA::TargetType::setType)
		.def("get_type", &SGA::TargetType::getType)
		.def("set_type", &SGA::TargetType::setType, py::arg("type"))

		.def_property("sampling_method", &SGA::TargetType::getSamplingMethod, &SGA::TargetType::setSamplingMethod)
		.def("get_sampling_method", &SGA::TargetType::getSamplingMethod)
		.def("set_sampling_method", &SGA::TargetType::setSamplingMethod, py::arg("samplingMethod"))

		.def_property("group_entity_types", &SGA::TargetType::getGroupEntityTypes, &SGA::TargetType::setGroupEntityTypes)
		.def("get_group_entity_types", &SGA::TargetType::getGroupEntityTypes)
		.def("set_group_entity_types", &SGA::TargetType::setGroupEntityTypes, py::arg("groupTypes"))

		.def("get_technology_types",
			[](SGA::TargetType& targetType)
			{
				return targetType.getTechnologyTypes();
			}
			)
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
		.def("get_spawneable_entities", &SGA::ActionTarget::getSpawnableEntities, py::arg("state"))

		//.def("get_position", &SGA::ActionTarget::getPosition, py::arg("state"))
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
		//.def_readwrite("grid", &SGA::Grid2D<SGA::Tile>::grid)
		.def("get", [](SGA::Grid2D<SGA::Tile>& b, int x, int y) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<SGA::Tile>::getWidth)
		.def("get_height", &SGA::Grid2D<SGA::Tile>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<SGA::Tile>::isInBounds, py::const_))
		;
	// ---- Grid2D<TileType> ----
	py::class_<SGA::Grid2D<std::shared_ptr<SGA::TileType>>>(m, "BoardTileTypes")
		.def(py::init<int, int, std::shared_ptr<SGA::TileType>>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
		//.def_readwrite("grid", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::grid)
		.def("get", [](const SGA::Grid2D<std::shared_ptr<SGA::TileType>>& b, int x, int y) {return b.get(x, y); })
		.def("get_width", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getWidth)
		.def("get_height", &SGA::Grid2D<std::shared_ptr<SGA::TileType>>::getHeight)
		.def("is_in_bounds", py::overload_cast<const SGA::Vector2i&>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		.def("is_in_bounds", py::overload_cast<int, int>(&SGA::Grid2D<std::shared_ptr<SGA::TileType>>::isInBounds, py::const_))
		;
	 
	// ---- GameRunner ----
	py::class_<SGA::GameRunner>(m, "GameRunner")
		.def("play",
			[](SGA::GameRunner& a, std::vector<std::shared_ptr<SGA::Agent>> newAgents, SGA::Vector2i& resolution,int seed = 0)
			{
				py::scoped_ostream_redirect stream(
					std::cout,                               // std::ostream&
					py::module_::import("sys").attr("stdout") // Python output
				);
				boost::mt19937 rngEngine(seed);
				
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
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
				a.play(newAgents.begin(), newAgents.end(), resolution);
				py::gil_scoped_acquire acquire;
			}
		)
		.def("play",
			[](SGA::GameRunner& a, py::list agents, SGA::Vector2i& resolution,int seed=0)
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

				boost::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
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
				a.play(newAgents.begin(), newAgents.end(), resolution);
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

				boost::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
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
				a.run(newAgents.begin(), newAgents.end());
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

				boost::mt19937 rngEngine(seed);
				// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
				boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
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
				a.run(newAgents.begin(), newAgents.end());
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

		.def("get_entity_type", &SGA::GameInfo::getEntityType, py::arg("entityTypeID"),"Returns the entity type.")
		.def("get_tile_type", &SGA::GameInfo::getTileType, py::arg("tileTypeID"), "Returns the tile type.")

		.def("get_parameter_global_id", &SGA::GameInfo::getParameterGlobalID, py::arg("parameterName"), "Returns the ID of the parameter.")
		.def("get_parameter_type", &SGA::GameInfo::getParameterType, py::arg("entityTypeID"), py::arg("globalParameterID"), "Returns the parameter type from a entityType.")
		.def("check_entity_have_parameter", &SGA::GameInfo::checkEntityHaveParameter, py::arg("entityTypeID"), py::arg("parameterName"), "Returns true if the entity type has the searched parameter")
		//.def("get_player_parameter", &SGA::GameInfo::getPlayerParameter, py::arg("id"), "Returns the parameter of the player.")

		.def("get_action_type_const", &SGA::GameInfo::getActionTypeConst, py::arg("actionTypeID"), "Returns the action type.")
		.def("get_action_type", &SGA::GameInfo::getActionType, py::arg("typeID"), "Returns the action type.")
		.def("get_action_type_id", &SGA::GameInfo::getActionTypeID, py::arg("actionName"),"Returns the action type ID.")

		.def("get_buff_type", &SGA::GameInfo::getBuffType, py::arg("buffTypeID"),"Returns the buff type by ID.")

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

		.def("set_yaml_path", &SGA::GameInfo::setYAMLPath,py::arg("path"))
		.def("set_technology_tree_collection", &SGA::GameInfo::setTechnologyTreeCollection,py::arg("tt"))
		.def("set_entity_groups", &SGA::GameInfo::setEntityGroups,py::arg("tg"))
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
		.def("can_player_lost", &SGA::ForwardModel::checkPlayerLost, py::arg("state"), py::arg("player"),"Checks if a player has lost the game due to the game's lose conditions.")
		.def("check_player_won", &SGA::ForwardModel::checkPlayerWon, py::arg("state"), py::arg("player"),"Returns if a player won the game by attending to the winning conditions defined in the rules.")
		.def("spawn_entity", &SGA::ForwardModel::spawnEntity, py::arg("state"), py::arg("entityType"), py::arg("playerID"), py::arg("position"),"Spawns an entity in the game with the default spawn entity method. A list of On Spawn effects are  executed just after spawning the entity")
		
		.def("add_win_conditions", &SGA::ForwardModel::addWinConditions, py::arg("conditions"), " Adds a list of conditions for the game to be won for a player.")
		.def("add_lose_conditions", &SGA::ForwardModel::addLoseConditions, py::arg("conditions"), "Adds a list of conditions for the game to be lost for a player.")
		.def("get_win_conditions", &SGA::ForwardModel::getWinConditions, "Returns a list of sub-lists with all WIN conditions. Each sub-list contains a group of conditions that must be fulfilled for the game to be over for a player. The game will be WON by a player if all conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. ")
		.def("get_lose_conditions", &SGA::ForwardModel::getLoseConditions, "Returns a list of sub-lists with all LOSE conditions. Each sub-list contains a group of conditions  that must be fulfilled for the game to be over for a player. The game will be LOST by a player if all  conditions in a sub-list are fullfiled, for at least one of the sub-lists returned. ")
		
		.def("add_on_tick_effect", &SGA::ForwardModel::addOnTickEffect, py::arg("ote"), "Adds an OnTickEffect to the forward mode, which will be executed every game tick.")
		.def("add_on_entity_spawn_effect", &SGA::ForwardModel::addOnEntitySpawnEffect, py::arg("ose"), "Adds an OnEntitySpawnEffect to the forward mode, which will be executed every time an entity is spawned.")
		
		.def("get_on_tick_effects", &SGA::ForwardModel::getOnTickEffects, "Returns all effects that are exxecuted on every tick of the game.")
		.def("get_on_entity_spawn_effects", &SGA::ForwardModel::getOnEntitySpawnEffects, "Returns all effects that are exxecuted every time an entity is spawned in the game.")
		;
		
		
	py::class_<SGA::TBSForwardModel, SGA::ForwardModel>(m, "TBSForwardModel")
		.def(py::init<>())

		.def("get_game_type", &SGA::TBSForwardModel::getGameType)

		.def("generate_actions", py::overload_cast<const SGA::GameState& , int>(&SGA::ForwardModel::generateActions, py::const_)," Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.")
		.def("generate_actions", py::overload_cast<const SGA::GameState& , int, std::vector<SGA::Action>&>(&SGA::ForwardModel::generateActions, py::const_), "Returns a list of available actions in the given gamestate by the received player")

		.def("advance_gamestate", py::overload_cast<SGA::GameState& , const SGA::Action&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes an action in a given SGA::GameState before updating the entities of the gamestate that should be removed and checking if the game is over.")
		.def("advance_gamestate", py::overload_cast<SGA::GameState& , const SGA::ActionAssignment&>(&SGA::TBSForwardModel::advanceGameState, py::const_), "Executes a list of actions.")

		.def("end_turn", &SGA::TBSForwardModel::endTurn, py::arg("state"),"End the turn of the current player and if all the player has played it ends the current game turn.")
		.def("check_game_is_finished", &SGA::TBSForwardModel::checkGameIsFinished, py::arg("state")," Checks if the game is finished by current limit or because a player has won.")
		;

	py::class_<SGA::RTSForwardModel, SGA::ForwardModel>(m, "RTSForwardModel")
		.def(py::init<>())
		.def("get_game_type", &SGA::RTSForwardModel::getGameType)

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
		//.def_readwrite("agent_name",&SGA::Agent::agentName)
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