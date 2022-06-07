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
	void misc(py::module_& m)
	{
		//---- Random Engine ----
		py::class_<boost::mt19937>(m, "mt19937");

		// ---- GameObserver ----
		py::class_<SGA::GameObserver>(m, "GameObserver");

		// ---- Timer ----
		py::class_<SGA::Timer>(m, "Timer")
			.def(py::init<long>(), py::arg("maxTimeMs") = 0)
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
}