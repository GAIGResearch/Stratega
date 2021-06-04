#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
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

#include "Stratega/Game/GameRunner.h"

namespace py = pybind11;

// STL
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Player>);
PYBIND11_MAKE_OPAQUE(std::vector<SGA::Entity>);
PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SGA::Agent>>);

 void test()
 {
	 std::cout << "Hello" << std::endl;
	 printf("Hello Stratega");
 }

void runDefault()
 {
	std::mt19937 rngEngine(0);
	std::string configPath("../../../gameConfigs/TBS/KillTheKing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);

	auto agents = gameConfig->generateAgents();
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

	auto gameRunner = SGA::createGameRunner(*gameConfig);
	gameRunner->play(agents);
 }

 std::unique_ptr<SGA::GameConfig> loadConfig(std::string& path)
 {
	 return SGA::loadConfigFromYAML(path);
 }

 std::unique_ptr<SGA::GameRunner> createRunner(const SGA::GameConfig* gameConfig)
 {
	 //py::list < std::unique_ptr<SGA::Agent> test;
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

	m.def("loadConfig", &loadConfig, "Loads game config", py::arg("path"));
	m.def("createRunner", &createRunner, "Create game runner", py::arg("gameConfig"));
	m.def("generateAgents", &generateAgents, "Generate agents", py::arg("gameConfig"));
	m.def("initializeAgents", &initializeAgents, "Initialize agents", py::arg("agents"), py::arg("seed"));

	m.def("test", &test,
		py::call_guard<py::scoped_ostream_redirect,
		py::scoped_estream_redirect>());
	m.def("runDefault", &runDefault, "Run default main");
 	
	// ---- STL ----
	py::bind_vector<std::vector<double>>(m, "DoubleList");
	py::bind_vector<std::vector<SGA::Player>>(m, "PlayerList");
	py::bind_vector<std::vector<SGA::Entity>>(m, "EntityList");
	py::bind_vector<std::vector<std::shared_ptr<SGA::Agent>>>(m, "AgentList");
	
	// ---- Vector2f ----
	py::class_<SGA::Vector2f>(m, "Vector2f")
		.def(py::init<double, double>(), py::arg("x") = 0.0, py::arg("y") = 0.0)
		.def_readwrite("x", &SGA::Vector2f::x)
		.def_readwrite("y", &SGA::Vector2f::y)
		.def("__repr__", [](const SGA::Vector2f& v) {return "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")"; });

	// ---- Vector2i ----
	py::class_<SGA::Vector2i>(m, "Vector2i")
		.def(py::init<double, double>(), py::arg("x") = 0.0, py::arg("y") = 0.0)
		.def_readwrite("x", &SGA::Vector2i::x)
		.def_readwrite("y", &SGA::Vector2i::y)
		.def("__repr__", [](const SGA::Vector2i& v) {return "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")"; });
	
	// ---- Entity ----
	py::class_<SGA::Entity>(m, "Entity")
		.def(py::init<>())
		//.def_readwrite("type_id", &SGA::Entity::typeID)
		.def_readwrite("id", &SGA::Entity::id)
		.def_readwrite("owner_ID", &SGA::Entity::ownerID)
		.def_readwrite("position", &SGA::Entity::position)
		//.def_readwrite("parameters", &SGA::Entity::parameters)
		.def_readwrite("sight_range", &SGA::Entity::lineOfSightRange)
		.def_readwrite("movement_speed", &SGA::Entity::movementSpeed)
		.def_readwrite("collision_radius", &SGA::Entity::collisionRadius)
		.def("is_neutral", &SGA::Entity::isNeutral);

	// ---- Player ----
	py::class_<SGA::Player>(m, "Player")
		.def(py::init<>())
		.def_readwrite("id", &SGA::Player::id)
		.def_readwrite("score", &SGA::Player::score)
		.def_readwrite("can_play", &SGA::Player::canPlay)
		.def_readwrite("parameters", &SGA::Player::parameters);

	// ---- Tile ----
	//py::class_<SGA::Tile>(m, "Tile")
	//	.def(py::init<int, int, int>(), py::arg("typeID"), py::arg("x"), py::arg("y"))
	//	//.def_readwrite("id", &SGA::Tile::tileTypeID)
	//	.def_readwrite("is_walkable", &SGA::Tile::isWalkable)
	//	.def_readwrite("blocks_sight", &SGA::Tile::blocksSight)
	//	.def_readwrite("position", &SGA::Tile::position);

	// ---- GameType ----
	py::enum_<SGA::GameType>(m, "GameType")
		.value("TBS", SGA::GameType::TBS)
		.value("RTS", SGA::GameType::RTS);

	// ---- Grid2D<Tile> ----
	//using Board = SGA::Grid2D<SGA::Tile>;
	//py::class_<Board>(m, "Board")
	//	.def(py::init<int, int, SGA::Tile>(), py::arg("width"), py::arg("height"), py::arg("default_tile"))
	//	.def("__get_item__", [](const Board& b, int y, int x) {return b.get(x, y); });
	
	// ---- GameState ----
	py::class_<SGA::GameState>(m, "GameState")
		.def(py::init([]()
		{
			std::cerr << "A GameState was constructed, use XXX instead" << std::endl;
			return std::make_unique<SGA::GameState>();
			}))
		.def_readwrite("current_player", &SGA::GameState::currentPlayer)
		.def_readwrite("game_type", &SGA::GameState::gameType)
		.def_readwrite("is_game_over", &SGA::GameState::isGameOver)
		.def_readwrite("winner_id", &SGA::GameState::winnerPlayerID)
		.def_readwrite("current_tick", &SGA::GameState::currentTick)
		.def_readwrite("tick_limit", &SGA::GameState::tickLimit)
		.def_readwrite("fog_of_war_id", &SGA::GameState::fogOfWarId)
		.def_readwrite("board", &SGA::GameState::board)
		.def_readwrite("entities", &SGA::GameState::entities)
		.def_readwrite("players", &SGA::GameState::players);

	// ---- GameConfig ----
	py::class_<SGA::GameConfig>(m, "GameConfig")
		.def("generateAgents", &SGA::GameConfig::generateAgents);

	// ---- GameRunner ----
	py::class_<SGA::GameRunner>(m, "GameRunner")
		.def("initializeAgents", &SGA::GameRunnerPy::initializeAgents)
		.def("play", &SGA::GameRunnerPy::play)
		//.def(py::pickle([](const SGA::GameRunner& p)
		//	{ // __getstate__
		//		/* Return a tuple that fully encodes the state of the object */
		//		return py::make_tuple("", "test");}))
		;

	// ---- Agent ----
	py::class_<SGA::Agent>(m, "Agent");

	// Add a scoped redirect for your noisy code
	m.def("noisy_func", []() {
		py::scoped_ostream_redirect stream(
			std::cout,                               // std::ostream&
			py::module_::import("sys").attr("stdout") // Python output
		);
		std::cout << "kjf";
		});
}