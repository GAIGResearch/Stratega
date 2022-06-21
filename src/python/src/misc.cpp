#include "headers.h"

namespace stratega
{
	void misc(py::module_& m)
	{
		//---- Random Engine ----
		py::class_<boost::mt19937>(m, "mt19937", "Random engine implementation.");

		// ---- GameObserver ----
		py::class_<SGA::GameObserver>(m, "GameObserver", "An interface for observing events in a game.");

		// ---- Timer ----
		py::class_<SGA::Timer>(m, "Timer", "Is an object that interacts with the game and responds to player input or other entities.")
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