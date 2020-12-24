#pragma once
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/StatSummary.h>

namespace SGA {

	class SimpleHeuristic : public StateHeuristic
	{
	private:
		int const playerID;
		int const numberOfPlayers;

	public:
		int const WIN_BOOST = 100000;
		int const LOOSE_BOOST = 100000;

	private:
		int score(const TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, int playerToScore) const;
		double calculateGridVisiblePercentage(TBSGameState2& gameState) const;

	public:
		SimpleHeuristic(int playerID, int numberOfPlayers);
		~SimpleHeuristic();

		double evaluateGameState(const TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, const int playerID) override;
	};

}

