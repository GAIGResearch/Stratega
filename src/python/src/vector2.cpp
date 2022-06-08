#include "headers.h"

namespace stratega
{
	void vector2(py::module_& m)
	{
		// ---- Vector2f ----
		py::class_<SGA::Vector2f>(m, "Vector2f", "Custom Vector class imlementation with float values.")
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
		py::class_<SGA::Vector2i>(m, "Vector2i", "Custom Vector class imlementation with intger values.")
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