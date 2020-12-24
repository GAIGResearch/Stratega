#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAbstractForwardModel.h>

namespace SGA
{
	class StateHeuristic
	{
	public:
		virtual ~StateHeuristic() = 0;

		virtual double evaluateGameState(const TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, const int playerID) { return 0; };
	};


	inline StateHeuristic::~StateHeuristic() = default;
}
