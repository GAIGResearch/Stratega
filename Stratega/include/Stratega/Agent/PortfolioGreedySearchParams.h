#pragma once
#include <vector>
#include <yaml-cpp/yaml.h>

#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

namespace  SGA
{
	struct PortfolioGreedySearchParams : AgentParameters {
		// additional agent parameters
		int NR_OF_TURNS_PLANNED = 3;			// the agents ID in the current game
		int ITERATIONS_PER_IMPROVE = 4;			// the number of iterations for one call of the improve method

		PortfolioGreedySearchParams()
		{
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			portfolio.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			portfolio.emplace_back(std::move(attackWeak));
			std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			portfolio.emplace_back(std::move(runAway));
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			portfolio.emplace_back(std::move(useSpecialAbility));
			std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			portfolio.emplace_back(std::move(runToFriends));
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			portfolio.emplace_back(std::move(random));
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