#pragma once
#include <math.h>
#include <random>

#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Agent/Heuristic/SimpleHeuristic.h>


namespace SGA {
    struct MCTSParams {
        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;
        int MAX_FM_CALLS = 2000;
        int REMAINING_FM_CALLS = MAX_FM_CALLS;
        bool FORCE_TURN_END = true;
        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;
        int playerID = 0;

        std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
        std::unique_ptr<StateHeuristic> STATE_HEURISTIC = std::make_unique<LinearSumHeuristic>();

        void printDetails() const;
    };
}

namespace YAML
{
    template<>
    struct convert<SGA::MCTSParameters>
    {
        static bool decode(const Node& node, SGA::MCTSParameters& rhs)
        {
            rhs.K = node["K"].as<double>(rhs.K);
            rhs.ROLLOUT_LENGTH= node["RolloutLength"].as<int>(rhs.ROLLOUT_LENGTH);
            rhs.ROLLOUTS_ENABLED = node["EnableRollouts"].as<bool>(rhs.ROLLOUTS_ENABLED);
        	rhs.MAX_FM_CALLS = node["FmCalls"].as<int>(rhs.MAX_FM_CALLS);
            rhs.REMAINING_FM_CALLS = rhs.MAX_FM_CALLS;
            return true;
        }
    };
}
