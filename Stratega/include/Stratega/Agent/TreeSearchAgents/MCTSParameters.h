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
        int rolloutLength = 3;
        bool rolloutsEnabled = true;
        double epsilon = 1e-2;
        bool continuePreviousSearch = true;

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
            rhs.decode(node);
            rhs.K = node["K"].as<double>(rhs.K);
            rhs.rolloutLength= node["RolloutLength"].as<int>(rhs.rolloutLength);
            rhs.rolloutsEnabled = node["EnableRollouts"].as<bool>(rhs.rolloutsEnabled);
            rhs.continuePreviousSearch = node["ContPreviousSearch"].as<bool>(rhs.continuePreviousSearch);
            return true;
        }
    };
}
