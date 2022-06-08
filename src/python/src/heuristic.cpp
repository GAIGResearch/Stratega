#include "headers.h"

namespace stratega
{
	void heuristic(py::module_& m)
	{
		// ---- Heuristic ----
		py::class_<SGA::MinimizeDistanceHeuristic>(m, "MinimizeDistanceHeuristic", "Heuristic that focus in minimizing the distance with the enemy.")
			.def(py::init<>())
			.def("get_name", &SGA::MinimizeDistanceHeuristic::getName)
			.def("evaluate_gamestate", &SGA::MinimizeDistanceHeuristic::evaluateGameState, py::arg("forwardModel"), py::arg("gameState"), py::arg("playerID"));

	}	
}