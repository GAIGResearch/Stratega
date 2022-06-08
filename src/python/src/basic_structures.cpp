#include "headers.h"

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



namespace stratega
{
	void basic_structures(py::module_& m)
	{
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
		py::bind_vector<std::vector<std::pair<SGA::Entity, std::vector<int>>>>(m, "Slots");

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

		// ---- SamplingMethod ----	
		py::class_<SGA::SamplingMethod, PySamplingMethod, std::shared_ptr<SGA::SamplingMethod>>(m, "SamplingMethod", "Contais the definition of the sampling method used to sample the targets.")
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
		py::class_<SGA::Condition, PyCondition, std::shared_ptr<SGA::Condition>>(m, "Condition", "An abstract class used to check a condition in a gamestate")
			.def("is_fullfiled", &SGA::Condition::isFullfiled, py::arg("state"), py::arg("targets"))
			.def("expr", &SGA::Condition::expr)
			;

		// ---- Effect ----	
		py::class_<SGA::Effect, PyEffect, std::shared_ptr<SGA::Effect>>(m, "Effect", "An abstract class used to modify the gamestate")
			.def("execute", &SGA::Effect::execute, py::arg("state"), py::arg("fm"), py::arg("targets"))
			.def("expr", &SGA::Effect::expr)
			;


		// ---- GameRunner ----
		py::class_<SGA::GameRunner>(m, "GameRunner", "An abstract class that provides a simplified interface to play games with a given SGA::GameConfig.")
			.def("play",
				[](SGA::GameRunner& a, std::vector<std::shared_ptr<SGA::Agent>> newAgents, SGA::Vector2i& resolution, int seed = 0)
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
				[](SGA::GameRunner& a, py::list agents, SGA::Vector2i& resolution, int seed = 0)
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

			std::cout << "Run GUI" << std::endl;
			py::gil_scoped_release release;
			a.play(newAgents.begin(), newAgents.end(), resolution);
			py::gil_scoped_acquire acquire;
		}
			)
			.def("run",
				[](SGA::GameRunner& a, py::list agents, int seed = 0)
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
				[](SGA::GameRunner& a, std::vector<std::shared_ptr<SGA::Agent>> newAgents, int seed = 0)
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
			.def("reset", py::overload_cast<int>(&SGA::GameRunner::reset), py::arg("levelID"), "Resets the game to an initial state.")
			.def("reset", py::overload_cast<>(&SGA::GameRunner::reset), "Resets the game to an initial state with a specific map.")
			.def("step", &SGA::GameRunner::step, py::arg("actions"), "Advances the game by one timestep. When the game has ended, you are responsible for calling GameRunner::reset() to reset the environments state.")
			.def("render", &SGA::GameRunner::render, "Renders a visual representation of the game. May create a new window when called for the first time.")
			.def("get_gamestate", &SGA::GameRunner::getGameState, "Returns a reference to the current state of the game.")
			;

		// ---- Agent ----
		py::class_<SGA::Agent, PyAgent, std::shared_ptr<SGA::Agent>/* <--- trampoline*/>(m, "Agent", "Abstract class from which all agents should inherit.")
			.def(py::init<std::string>(), py::arg("name") = "PythonAgent")
			//.def_readwrite("agent_name",&SGA::Agent::agentName)
			.def("computeAction", &SGA::Agent::computeAction, "Function for deciding the next action to execute. Must be overriden for an agent to work. Returns an ActionAssignment")
			.def("init", &SGA::Agent::init, "Function for initializing the agent. Override this function to receive a call just before starts.")
			.def("get_player_id", &SGA::Agent::getPlayerID);

		// ---- Arena ----
		py::class_<Arena>(m, "Arena","The Arena provides an easy way to test the performance between different Agents in different environments.")
			.def("run_games", py::overload_cast<int, int, int, int>(&Arena::runGames))
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
	}	
}