#include "headers.h"

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