#include <Stratega/Agent/RHEAAgent/RHEAParameters.h>

#include <iostream>


namespace SGA {
	void RHEAParameters::printDetails() const
	{
		AgentParameters::printDetails();
		std::cout << "RHEA Parameters:" << std::endl;
		std::cout << "\tPopulation size: " << popSize << std::endl;
		std::cout << "\tIndividual length: " << individualLength << std::endl;
		std::cout << "\tMutation rate: " << mutationRate << std::endl;
		std::cout << "\tTournament size: " << tournamentSize << std::endl;
		std::cout << "\tElitism: " << elitism << std::endl;
		std::cout << "\tContinue search: " << (continuePreviousSearch ? "True" : "False") << std::endl;
		std::cout << "\tMutate best individual count: " << mutateBestN << std::endl;
		std::cout << "\tEpsilon: " << epsilon << std::endl;
	}
}

