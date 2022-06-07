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
	void buff(py::module_& m)
	{
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
	}	
}