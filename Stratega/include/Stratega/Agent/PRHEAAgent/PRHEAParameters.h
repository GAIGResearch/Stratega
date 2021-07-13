#pragma once
#include <vector>
#include <yaml-cpp/yaml.h>

#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>



namespace SGA {
	struct PRHEAParameters : AgentParameters {
		// basic parameters
		size_t POP_SIZE = 1;				// population size
		size_t INDIVIDUAL_LENGTH = 1;		// planning horizon of an individual

		// evolution and selection
		double MUTATION_RATE = 0.5;			// mutation rate when transferring one individual to the next generation
		int TOURNAMENT_SIZE = 5;			// number of individuals per tournament selection
		bool ELITISM = true;				// if true, always transfer the best individual to the next generation

		// re-use previous iteration?
		bool CONTINUE_SEARCH = true;		// initialize new population with shifted best individual of the previous iteration
		size_t MUTATE_BEST = 9;				// include Mutate_best additional copies of the shifted best individual in the next population
		
		double EPSILON = 1e-2;									// the amount of noise for randomly modifying an individuals value

		PRHEAParameters() {
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
	struct convert<SGA::PRHEAParameters>
	{
		static bool decode(const Node& node, SGA::PRHEAParameters& rhs)
		{
			rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
			rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
			return true;
		}
	};
}