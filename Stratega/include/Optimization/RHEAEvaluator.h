#pragma once
#include <Optimization/Evaluator.h>

namespace SGA
{
    /// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class RHEAEvaluator : Evaluator
    {
    private:
        std::string _name;

    public:
        RHEAEvaluator(std::string name);

        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        void printPoint(const std::vector<int>& point) override;
    };
}
