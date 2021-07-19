#pragma once
#include <random>
#include <yaml-cpp/yaml.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
    struct MCTSParameters : AgentParameters
	{

    public:

        MCTSParameters() 
        {
        }

        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;
        bool FORCE_TURN_END = true;
        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;
        bool CONTINUE_PREVIOUS_SEARCH = true;

        std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);

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

        	rhs.maxFMCalls = node["FmCalls"].as<int>(rhs.maxFMCalls);
            rhs.maxIterations = node["Iterations"].as<int>(rhs.maxIterations);
            if (node["Budget"].as<std::string>("") == "TIME")
                rhs.budgetType = SGA::Budget::TIME;
            else if (node["Budget"].as<std::string>("") == "FMCALLS")
                rhs.budgetType = SGA::Budget::FMCALLS;
            else if (node["Budget"].as<std::string>("") == "ITERATIONS")
                rhs.budgetType = SGA::Budget::ITERATIONS;

            return true;
        }
    };
}
