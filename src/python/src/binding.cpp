

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

namespace stratega
{
	void actions(py::module_& m);
	void basic_structures(py::module_& m);
	void buff(py::module_& m);
	void default_methods(py::module_& m);
	void entity(py::module_& m);
	void forwardmodel(py::module_& m);
	void game(py::module_& m);
	void gamestate(py::module_& m);
	void heuristic(py::module_& m);
	void misc(py::module_& m);
	void parameter(py::module_& m);
	void player(py::module_& m);
	void technology(py::module_& m);
	void tile(py::module_& m);
	void vector2(py::module_& m);	
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

	stratega::entity(m);

	stratega::basic_structures(m);
	
	stratega::actions(m);
	stratega::buff(m);
	stratega::default_methods(m);	
	stratega::forwardmodel(m);
	stratega::game(m);
	stratega::gamestate(m);
	stratega::heuristic(m);
	stratega::misc(m);
	stratega::parameter(m);
	stratega::player(m);
	stratega::technology(m);
	stratega::tile(m);
	stratega::vector2(m);
}