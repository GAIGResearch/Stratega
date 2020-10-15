#pragma once
#include <Agent/Heuristic/LinearSumHeuristic.h>

#include "Agent/Portfolios/AttackClosest.h"

namespace SGA {
	struct RHEAParams {
		// basic parameters
		size_t POP_SIZE = 10;				// population size
		size_t INDIVIDUAL_LENGTH = 10;		// planning horizon of an individual

		// evolution and selection
		double MUTATION_RATE = 0.1;			// mutation rate when transferring one individual to the next generation
		int TOURNAMENT_SIZE = 3;			// number of individuals per tournament selection
		bool ELITISM = true;				// if true, always transfer the best individual to the next generation

		// re-use previous iteration?
		bool CONTINUE_SEARCH = true;		// initialize new population with shifted best individual of the previous iteration
		size_t MUTATE_BEST = 9;				// include Mutate_best additional copies of the shifted best individual in the next population

		// agent parameters
		int MAX_FM_CALLS = 2000;				// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game

		LinearSumHeuristic HEURISTIC{ LinearSumHeuristic() };	// the heuristic used to evaluate a genome
		double EPSILON = 1e-2;									// the amount of noise for randomly modifying an individuals value
		std::unique_ptr<BasePortfolio> opponentModel = std::make_unique<AttackClosest>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		void printDetails() const;
	};
}
