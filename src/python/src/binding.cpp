#include "headers.h"

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
			ActionFlag
			ActionInfo
			Action

			EntityType
			Entity

			TileType
			Tile
			BoardTiles
			Player

			GameType
			GameDescription
			EntityCategory
			ActionCategory
						
			Parameter
			
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
			
			GameRunner
			GameConfig
			GameInfo

			Arena

			GameObserver
			MinimizeDistanceHeuristic
			ActionAssignment
			
			GameState

			EntityForwardModel
			TBSForwardModel
			RTSForwardModel
			OnTickEffect
			OnEntitySpawnEffect

			Path
			Agent
			
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