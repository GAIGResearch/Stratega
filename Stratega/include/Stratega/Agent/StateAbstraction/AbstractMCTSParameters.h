#pragma once
#include <random>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

#include <yaml-cpp/yaml.h>

#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>


namespace SGA
{
	struct AbstractMCTSParameters : AgentParameters
	{
		double K = sqrt(2);
		int ROLLOUT_LENGTH = 3;
		bool ROLLOUTS_ENABLED = true;
		bool FORCE_TURN_END = true;
		bool PRIORITIZE_ROOT = true;
		double EPSILON = 1e-2;

		bool CONTINUE_PREVIOUS_SEARCH = true;

		std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);

		std::unique_ptr<StateHeuristic> STATE_HEURISTIC = nullptr;

		void printDetails() const;

		AbstractMCTSParameters() {
			/*
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackClose));
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
			*/
		}
	};
}

namespace YAML
{
	template<>
	struct convert<SGA::AbstractMCTSParameters>
	{
		static bool decode(const Node& node, SGA::AbstractMCTSParameters& rhs)
		{
			rhs.K = node["K"].as<double>(rhs.K);
			rhs.ROLLOUT_LENGTH = node["RolloutLength"].as<int>(rhs.ROLLOUT_LENGTH);
			rhs.ROLLOUTS_ENABLED = node["EnableRollouts"].as<bool>(rhs.ROLLOUTS_ENABLED);
			rhs.FORCE_TURN_END = node["ForceTurnEnd"].as<bool>(rhs.FORCE_TURN_END);
			rhs.PRIORITIZE_ROOT = node["PrioritizeRoot"].as<bool>(rhs.PRIORITIZE_ROOT);
			rhs.EPSILON = node["Epsilon"].as<bool>(rhs.EPSILON);
			rhs.CONTINUE_PREVIOUS_SEARCH = node["ContinuePreviousSearch"].as<bool>(rhs.CONTINUE_PREVIOUS_SEARCH);
			
			rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
			rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
			return true;
		}
	};
}