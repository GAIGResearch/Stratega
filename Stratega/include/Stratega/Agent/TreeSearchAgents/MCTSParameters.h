#pragma once
#include <random>
#include <yaml-cpp/yaml.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
    struct MCTSParameters : AgentParameters
	{
        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;
        bool FORCE_TURN_END = true;
        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;
    	
        bool CONTINUE_PREVIOUS_SEARCH = false;

        std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);

        std::unique_ptr<StateHeuristic> STATE_HEURISTIC = nullptr;

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
