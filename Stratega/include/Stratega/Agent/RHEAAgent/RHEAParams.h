#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>
#include <yaml-cpp/yaml.h>

namespace SGA {
	struct RHEAParams : AgentParameters {

	public:

		RHEAParams()
		{
			opponentModel = std::make_shared<RandomActionScript>();
			heuristic = std::make_shared<MinimizeDistanceHeuristic>();
		}

		// basic parameters
		size_t popSize = 10;				// population size
		size_t individualLength = 10;		// planning horizon of an individual

		// evolution and selection
		double mutationRate = 0.1;			// mutation rate when transferring one individual to the next generation
		int tournamentSize = 3;			// number of individuals per tournament selection
		bool elitism = true;				// if true, always transfer the best individual to the next generation

		// re-use previous iteration?
		bool continuePreviousSearch = true;		// initialize new population with shifted best individual of the previous iteration
		size_t mutateBestN = 9;				// include Mutate_best additional copies of the shifted best individual in the next population

		double epsilon = 1e-2;					// the amount of noise for randomly modifying an individuals value
		
		void printDetails() const;
	};
}


namespace YAML
{
	template<>
	struct convert<SGA::RHEAParams>
	{
		static bool decode(const Node& node, SGA::RHEAParams& rhs)
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
