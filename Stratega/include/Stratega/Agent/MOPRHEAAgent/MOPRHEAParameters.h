#pragma once
#include <vector>

#include <Stratega/Agent/PRHEAAgent/PRHEAParameters.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>

#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>



namespace SGA {
	struct MOPRHEAParameters : PRHEAParameters {
		
		std::shared_ptr<StateHeuristic>  heuristic2 = std::make_unique<MinimizeDistanceHeuristic>();	// the heuristic used to evaluate a genome

		MOPRHEAParameters() {
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
		
		void printDetails() const;
	};
}

namespace YAML
{
	template<>
	struct convert<SGA::MOPRHEAParameters>
	{
		static bool decode(const Node& node, SGA::MOPRHEAParameters& rhs)
		{
			rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
			rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
			return true;
		}
	};
}