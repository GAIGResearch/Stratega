#pragma once
#include <random>
#include <Stratega/Configuration/YamlHeaders.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>


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

		std::unique_ptr<StateHeuristic> STATE_HEURISTIC = std::make_unique<MinimizeDistanceHeuristic>();
		std::unique_ptr<BaseActionScript> opponentModel = std::make_unique<RandomActionScript>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		void printDetails() const;
	};
}