#pragma once
#include <Evaluators/Evaluator.h>
#include <vector>
#include <VectorSearchSpace.h>

namespace SGA
{

    /// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class NMaxEvaluator : public Evaluator
    {
    private:
        std::string _name;

    public:
        NMaxEvaluator(int dim, int sizePerDim);


        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        void printPoint(const std::vector<int>& point) override;

    };

}
