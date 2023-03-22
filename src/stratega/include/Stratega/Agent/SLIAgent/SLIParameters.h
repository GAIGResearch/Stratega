#pragma once
#include <boost/random.hpp>
#include <yaml-cpp/yaml.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
	struct SLIParameters : AgentParameters
	{

	public:

		SLIParameters() {}

		int step = 0; //for debug
		
		int generateMaxFMCalls = maxFMCalls * 0.5;
		int evaluateMaxFMCalls = maxFMCalls;

		std::vector<SGA::Action> actionSequence = {};
		
		/*parameters for the SLIAgent*/
		int nSample = 100;
		int N_SAMPLE_FOR_EVALUATE = 20;
		int EVALUATE_CONSTANT = 100;

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

		bool isGenerateBudgetOver() const;
		bool isEvaluateBudgetOver() const;

		std::string intVectorToString(std::vector<int> vec);
	};
}

namespace YAML
{
	template<>
	struct convert<SGA::SLIParameters>
	{
		/// <summary>
		/// Reads Agent generic (via call rhs.decode(node)) and MCTS-specific parameters received in a YAML node.
		/// Add lines to this method to capture more parameters.
		/// </summary>
		/// <param name="node">YAML Node with the parameters for the agent.</param>
		/// <param name="rhs">Parameters object to be modified with the contents of the YAML node.</param>
		/// <returns>True if there was no problem.</returns>
		static bool decode(const Node& node, SGA::SLIParameters& rhs)
		{
			rhs.decode(node);
			rhs.K = node["K"].as<double>(rhs.K);
			rhs.rolloutLength = node["RolloutLength"].as<int>(rhs.rolloutLength);
			rhs.rolloutsEnabled = node["EnableRollouts"].as<bool>(rhs.rolloutsEnabled);
			rhs.continuePreviousSearch = node["ContinuePreviousSearch"].as<bool>(rhs.continuePreviousSearch);
			rhs.nSample = node["NSample"].as<int>(rhs.nSample);
			rhs.N_SAMPLE_FOR_EVALUATE = node["NSampleForEvaluate"].as<int>(rhs.N_SAMPLE_FOR_EVALUATE);
			rhs.EVALUATE_CONSTANT = node["EvaluateConstant"].as<int>(rhs.EVALUATE_CONSTANT);
			return true;
		}
	};
}
