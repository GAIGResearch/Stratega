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
	void vector2(py::module_& m)
	{
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
	}	
}