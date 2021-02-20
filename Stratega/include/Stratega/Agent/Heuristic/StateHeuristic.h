#pragma once
#include <Stratega/Representation/TBSGameState.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>

namespace SGA
{
	class StateHeuristic
	{
	public:
		virtual ~StateHeuristic() = 0;

		virtual double evaluateGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const int playerID) { return 0; };
	};


	inline StateHeuristic::~StateHeuristic() = default;
}
