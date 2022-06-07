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
	void parameter(py::module_& m)
	{
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
	}	
}