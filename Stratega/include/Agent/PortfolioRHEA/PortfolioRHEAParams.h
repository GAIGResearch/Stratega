#pragma once
#include <vector>

#include <Agent/Heuristic/LinearSumHeuristic.h>

#include <Agent/ActionScripts/BaseActionScript.h>
#include <Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Agent/ActionScripts/RandomActionScript.h>



namespace SGA {
	struct PortfolioRHEAParams {
		// basic parameters
		size_t POP_SIZE = 100;				// population size
		size_t INDIVIDUAL_LENGTH = 3;		// planning horizon of an individual

		// evolution and selection
		double MUTATION_RATE = 0.5;			// mutation rate when transferring one individual to the next generation
		int TOURNAMENT_SIZE = 5;			// number of individuals per tournament selection
		bool ELITISM = true;				// if true, always transfer the best individual to the next generation

		// re-use previous iteration?
		bool CONTINUE_SEARCH = true;		// initialize new population with shifted best individual of the previous iteration
		size_t MUTATE_BEST = 9;				// include Mutate_best additional copies of the shifted best individual in the next population

		// agent parameters
		int MAX_FM_CALLS = 2000;				// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game
		
		std::vector<std::unique_ptr<BaseActionScript>> PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();	// the PORTFOLIO used to sample actions of a genome
		LinearSumHeuristic HEURISTIC{ LinearSumHeuristic() };	// the heuristic used to evaluate a genome
		double EPSILON = 1e-2;									// the amount of noise for randomly modifying an individuals value
		std::unique_ptr<BaseActionScript> opponentModel = std::make_unique<AttackClosestOpponentScript>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		PortfolioRHEAParams() {
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackWeak));
			//std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			//PORTFOLIO.emplace_back(std::move(runAway));
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			PORTFOLIO.emplace_back(std::move(useSpecialAbility));
			//std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			//PORTFOLIO.emplace_back(std::move(runToFriends));
			//std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			//PORTFOLIO.emplace_back(std::move(random));
		}
		
		void printDetails() const;
	};
}

namespace YAML
{
	template<>
	struct convert<SGA::PortfolioRHEAParams>
	{
		static bool decode(const Node& node, SGA::PortfolioRHEAParams& rhs)
		{
			rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
			rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
			return true;
		}
	};
}