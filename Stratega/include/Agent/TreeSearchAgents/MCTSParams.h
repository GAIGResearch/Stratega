#pragma once
#include <math.h>
#include <random>

#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Agent/Heuristic/SimpleHeuristic.h>

#include <Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Agent/AgentParameters.h>

namespace SGA {
    struct MCTSParams : AgentParameters {
        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;
        bool FORCE_TURN_END = true;
        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;
        int playerID = 0;

        std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
        std::unique_ptr<StateHeuristic> STATE_HEURISTIC = std::make_unique<LinearSumHeuristic>();
        std::unique_ptr<BaseActionScript> opponentModel = std::make_unique<AttackClosestOpponentScript>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

        void printDetails() const;
    };
}
