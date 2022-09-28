#pragma once
#include <boost/random.hpp>
#include <yaml-cpp/yaml.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
    struct MCTSParameters : AgentParameters
	{

    public:

        MCTSParameters() {}

        int step = 0; //for debug

        double K = sqrt(2);                     //Balance constant for tree policy (UCT)
        int rolloutLength = 3;                  //Lenght of the complete playout.
        bool rolloutsEnabled = true;            //If the simulation/rollout phase should be executed or not.
        bool continuePreviousSearch = true;     //Indicates if tree should be kept between two consecutive decision making steps.

        boost::random::uniform_real_distribution<double> doubleDistribution_ = boost::random::uniform_real_distribution<double>(0, 1);  //Uniform distribution of real numbers in [0,1]
        double epsilon = 1e-2;                  //Small number to avoid /0

        /// <summary>
        /// Prints the details of this parameters object.
        /// </summary>

        void printDetails() const;
    };
}

namespace YAML
{	
    template<>
    struct convert<SGA::MCTSParameters>
    {
        /// <summary>
        /// Reads Agent generic (via call rhs.decode(node)) and MCTS-specific parameters received in a YAML node.
        /// Add lines to this method to capture more parameters.
        /// </summary>
        /// <param name="node">YAML Node with the parameters for the agent.</param>
        /// <param name="rhs">Parameters object to be modified with the contents of the YAML node.</param>
        /// <returns>True if there was no problem.</returns>
        static bool decode(const Node& node, SGA::MCTSParameters& rhs)
        {
            rhs.decode(node);
            rhs.K = node["K"].as<double>(rhs.K);
            rhs.rolloutLength= node["RolloutLength"].as<int>(rhs.rolloutLength);
            rhs.rolloutsEnabled = node["EnableRollouts"].as<bool>(rhs.rolloutsEnabled);
            rhs.continuePreviousSearch = node["ContinuePreviousSearch"].as<bool>(rhs.continuePreviousSearch);
            return true;
        }
    };
}
