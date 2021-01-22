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

namespace  SGA
{
	struct PortfolioGreedySearchParams {
		// agent parameters
		int MAX_FM_CALLS = 2000;				// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game
		int NR_OF_TURNS_PLANNED = 3;			// the agents ID in the current game
		int ITERATIONS_PER_IMPROVE = 1;			// the number of iterations for one call of the improve method

		std::vector<std::unique_ptr<BaseActionScript>> PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();	// the PORTFOLIO used to sample actions of a genome
		LinearSumHeuristic HEURISTIC{ LinearSumHeuristic() };	// the heuristic used to evaluate a genome
		PortfolioGreedySearchParams()
		{
			
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackWeak));
			std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			PORTFOLIO.emplace_back(std::move(runAway));
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			PORTFOLIO.emplace_back(std::move(useSpecialAbility));
			std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			PORTFOLIO.emplace_back(std::move(runToFriends));
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			PORTFOLIO.emplace_back(std::move(random));
		}
	};
}

namespace YAML
{
	template<>
	struct convert<SGA::PortfolioGreedySearchParams>
	{
		static bool decode(const Node& node, SGA::PortfolioGreedySearchParams& rhs)
		{
			rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
			rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
			return true;
		}
	};
}