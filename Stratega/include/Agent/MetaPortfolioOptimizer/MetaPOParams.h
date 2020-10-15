#pragma once
#include <vector>

#include <Agent/Heuristic/LinearSumHeuristic.h>

#include <Agent/Portfolios/BasePortfolio.h>
#include <Agent/Portfolios/AttackClosest.h>
#include <Agent/Portfolios/AttackWeakest.h>
#include <Agent/Portfolios/RunAway.h>
#include <Agent/Portfolios/UseSpecialAbility.h>
#include <Agent/Portfolios/RunToFriends.h>
#include <Agent/Portfolios/RandomPortfolio.h>



namespace SGA {
	struct MetaPOParams {
		// basic parameters
		size_t POP_SIZE = 10;				// population size
		int HORIZON = 5;					// planning horizon of an individual (number of completed turns)
		int NR_OF_SCRIPT_CHANGES = 3;	    // number of script changes per genome, each script change updates the unit script assignment in a certain point in time

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
		
		std::vector<std::unique_ptr<BasePortfolio>> portfolios = std::vector<std::unique_ptr<BasePortfolio>>();	// the portfolios used to sample actions of a genome
		std::unique_ptr<BasePortfolio> opponentModel = std::make_unique<AttackClosest>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped
		//std::unique_ptr<BasePortfolio> opponentModel = nullptr;	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped
		LinearSumHeuristic HEURISTIC{ LinearSumHeuristic() };	// the heuristic used to evaluate a genome
		double EPSILON = 1e-2;									// the amount of noise for randomly modifying an individuals value

		MetaPOParams() {
			std::unique_ptr<BasePortfolio> attackClose = std::make_unique<AttackClosest>();
			portfolios.emplace_back(std::move(attackClose));
			std::unique_ptr<BasePortfolio> attackWeak = std::make_unique<AttackWeakest>();
			portfolios.emplace_back(std::move(attackWeak));
			std::unique_ptr<BasePortfolio> runAway = std::make_unique<RunAway>();
			portfolios.emplace_back(std::move(runAway));
			std::unique_ptr<BasePortfolio> useSpecialAbility = std::make_unique<UseSpecialAbility>();
			portfolios.emplace_back(std::move(useSpecialAbility));
			std::unique_ptr<BasePortfolio> runToFriends = std::make_unique<RunToFriends>();
			portfolios.emplace_back(std::move(runToFriends));
			std::unique_ptr<BasePortfolio> random = std::make_unique<RandomPortfolio>();
			portfolios.emplace_back(std::move(random));
		}
		
		void printDetails() const;
	};
}
