#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/StatSummary.h>

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
		int score(const ForwardModel& forwardModel, GameState& gameState, int playerToScore) const;
		double calculateGridVisiblePercentage(GameState& gameState) const;

	public:
		SimpleHeuristic(int playerID, int numberOfPlayers);
		~SimpleHeuristic();

		double evaluateGameState(const ForwardModel& forwardModel, GameState& gameState, const int playerID) override;
	};

}

