#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>
#include <yaml-cpp/yaml.h>
#include <boost/random.hpp>

namespace SGA {
	struct RHEAParameters : AgentParameters {

	public:

		RHEAParameters() { }

		size_t popSize = 10;				// population size
		size_t individualLength = 10;		// planning horizon of an individual
		double mutationRate = 0.1;			// mutation rate when transferring one individual to the next generation
		int tournamentSize = 3;				// number of individuals per tournament selection
		bool elitism = true;				// if true, always transfer the best individual to the next generation
		bool continuePreviousSearch = true;	// initialize new population with shifted best individual of the previous iteration
		size_t mutateBestN = 9;				// include Mutate_best additional copies of the shifted best individual in the next population

		double epsilon = 1e-2;				// the amount of noise for randomly modifying an individuals value
		boost::random::uniform_real_distribution<double> doubleDistribution_ = boost::random::uniform_real_distribution<double>(0, 1);  //Uniform distribution of real numbers in [0,1]
		
		void printDetails() const;
	};
}


namespace YAML
{
	template<>
	struct convert<SGA::RHEAParameters>
	{
		/// <summary>
		/// Reads Agent generic (via call rhs.decode(node)) and RHEA-specific parameters received in a YAML node.
		/// Add lines to this method to capture more parameters.
		/// </summary>
		/// <param name="node">YAML Node with the parameters for the agent.</param>
		/// <param name="rhs">Parameters object to be modified with the contents of the YAML node.</param>
		/// <returns>True if there was no problem.</returns>
		static bool decode(const Node& node, SGA::RHEAParameters& rhs)
		{
			rhs.decode(node);
			rhs.popSize = node["PopSize"].as<size_t>(rhs.popSize);
			rhs.individualLength = node["IndLength"].as<size_t>(rhs.individualLength);
			rhs.mutationRate = node["MutationRate"].as<double>(rhs.mutationRate);
			rhs.tournamentSize = node["TournamentSize"].as<int>(rhs.tournamentSize);
			rhs.elitism = node["Elitism"].as<bool>(rhs.elitism);
			rhs.continuePreviousSearch = node["ContPreviousSearch"].as<bool>(rhs.continuePreviousSearch);
			rhs.mutateBestN = node["MutateBestN"].as<size_t>(rhs.mutateBestN);
			return true;
		}
	};
}
