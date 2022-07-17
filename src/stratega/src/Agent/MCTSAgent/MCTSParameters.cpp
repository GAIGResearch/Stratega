#include <Stratega/Agent/MCTSAgent/MCTSParameters.h>

#include <iostream>


namespace SGA {
	void MCTSParameters::printDetails() const
	{
		AgentParameters::printDetails();
		std::cout << "MCTS Parameters:" << "\n";
		std::cout << "\tK: " << K << "\n";
		std::cout << "\tRollout length: " << rolloutLength << "\n";
		std::cout << "\tRollouts enabled: " << (rolloutsEnabled ? "True" : "False") << "\n";
		std::cout << "\tEpsilon = " << epsilon << "\n";
		std::cout << "\tContinue Previous Search = " << continuePreviousSearch << "\n";
	}
}