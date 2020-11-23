#include <Optimization/RHEAEvaluator.h>

namespace SGA
{
    RHEAEvaluator::RHEAEvaluator(std::string name) : Evaluator(std::move(name)) {};

    std::vector<float> RHEAEvaluator::evaluate(std::vector<int> point, int nSamples)
    {
		// todo: evaluate the agent   
    }

	void RHEAEvaluator::printPoint(const std::vector<int>& point)
    {
        for (const int i : point)
	        std::cout << i;
    }
}
