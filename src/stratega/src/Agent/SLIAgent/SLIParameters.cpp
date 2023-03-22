#include <Stratega/Agent/SLIAgent/SLIParameters.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>


namespace SGA {
	void SLIParameters::printDetails() const
	{
		AgentParameters::printDetails();
		std::cout << "SLI Parameters:" << "\n";
		std::cout << "\tRollout length: " << rolloutLength << "\n";
		std::cout << "\tnSample = " << nSample << "\n";
		std::cout << "\tN_SAMPLE_FOR_EVALUATE = " << N_SAMPLE_FOR_EVALUATE << "\n";
		std::cout << "\tEVALUATE_CONSTANT = " << EVALUATE_CONSTANT << "\n";
	}

	//template<typename T>
	std::string SLIParameters::intVectorToString(std::vector<int> vec) {
		if (vec.empty()) {
			return std::string();
		}

		return std::accumulate(vec.begin() + 1, vec.end(), std::to_string(vec[0]),
			[](const std::string& a, int b) {
				return a + std::to_string(b);
			});
	}

	bool SLIParameters::isGenerateBudgetOver() const
	{
		return currentFMCalls >= generateMaxFMCalls;
	}

	bool SLIParameters::isEvaluateBudgetOver() const
	{
		return currentFMCalls >= evaluateMaxFMCalls;
	}
}