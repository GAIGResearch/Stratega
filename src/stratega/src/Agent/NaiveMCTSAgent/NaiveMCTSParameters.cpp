#include <Stratega/Agent/NaiveMCTSAgent/NaiveMCTSParameters.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>


namespace SGA {
void NaiveMCTSParameters::printDetails() const
	{
		AgentParameters::printDetails();
		std::cout << "NaiveMCTS Parameters:" << "\n";
		std::cout << "\tK: " << K << "\n";
		std::cout << "\tRollout length: " << rolloutLength << "\n";
		std::cout << "\tRollouts enabled: " << (rolloutsEnabled ? "True" : "False") << "\n";
		std::cout << "\tEpsilon = " << epsilon << "\n";
		std::cout << "\tContinue Previous Search = " << continuePreviousSearch << "\n";
		std::cout << "\tMax depth = " << maxDepth << "\n";
		std::cout << "\tEpsilon_0 = " << epsilon_0 << "\n";
		std::cout << "\tEpsilon_g = " << epsilon_g << "\n";
		std::cout << "\tEpsilon_l = " << epsilon_l << "\n";
	}

//template<typename T>
std::string NaiveMCTSParameters::intVectorToString(std::vector<int> vec){
	if (vec.empty()) {
		return std::string();
	}

	return std::accumulate(vec.begin() + 1, vec.end(), std::to_string(vec[0]),
		[](const std::string& a, int b) {
			return a + std::to_string(b);
		});
	}
}