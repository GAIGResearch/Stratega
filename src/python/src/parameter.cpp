#include "headers.h"

namespace stratega
{
	void parameter(py::module_& m)
	{
		// ---- Parameter ----
		py::class_<SGA::Parameter>(m, "Parameter", "Contains all the information of a parameter such as the default, min and max value.")
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