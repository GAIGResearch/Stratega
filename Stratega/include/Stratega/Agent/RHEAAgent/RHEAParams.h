#pragma once
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
	struct RHEAParams : AgentParameters {

	public:

		RHEAParams()
		{
			opponentModel = std::make_shared<RandomActionScript>();
			heuristic = std::make_shared<MinimizeDistanceHeuristic>();
		}

		// basic parameters
		size_t POP_SIZE = 10;				// population size
		size_t INDIVIDUAL_LENGTH = 10;		// planning horizon of an individual

		// evolution and selection
		double MUTATION_RATE = 0.1;			// mutation rate when transferring one individual to the next generation
		int TOURNAMENT_SIZE = 3;			// number of individuals per tournament selection
		bool ELITISM = true;				// if true, always transfer the best individual to the next generation

		// re-use previous iteration?
		bool CONTINUE_SEARCH = true;		// initialize new population with shifted best individual of the previous iteration
		size_t MUTATE_BEST = 9;				// include Mutate_best additional copies of the shifted best individual in the next population

		double EPSILON = 1e-2;					// the amount of noise for randomly modifying an individuals value
		
		void printDetails() const;
	};
}
