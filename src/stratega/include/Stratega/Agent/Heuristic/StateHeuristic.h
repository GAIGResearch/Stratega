#pragma once
#include <Stratega/ForwardModel/ForwardModel.h>

namespace SGA
{
	class StateHeuristic
	{
	public:
		virtual ~StateHeuristic() = 0;

		virtual double evaluateGameState(const ForwardModel& /*forwardModel*/, GameState& /*gameState*/, const int /*playerID*/) { return 0; };
		virtual std::string getName() const { return "Undefined heuristic name"; }
	};


	inline StateHeuristic::~StateHeuristic() = default;
}
