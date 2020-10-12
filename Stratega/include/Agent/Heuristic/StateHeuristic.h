#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{
	class StateHeuristic
	{
	public:
		virtual ~StateHeuristic() = 0;

		virtual double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState) { return 0; };
	};


	inline StateHeuristic::~StateHeuristic() = default;
}
