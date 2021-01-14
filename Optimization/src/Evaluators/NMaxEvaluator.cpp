#include <Evaluators/NMaxEvaluator.h>


namespace SGA
{

    NMaxEvaluator::NMaxEvaluator(int dim, int sizePerDim) :
	Evaluator("NMaxEvaluator")
    {
        std::vector<int> searchSpaceDims;
    	for (int i = 0; i < dim; i++)
    	{
            searchSpaceDims.emplace_back(sizePerDim);
    	}
        _searchSpace = std::make_unique<VectorSearchSpace>(searchSpaceDims);
    }


    std::vector<float> NMaxEvaluator::evaluate(std::vector<int> point, int nSamples)
    {
        int sum = 0;
    	for (int& i : point)
    	{
            sum += i;
    	}

        std::vector<float> result;
    	for (int i = 0; i < nSamples; i++)
    	{
            result.emplace_back(sum);
    	}
        return result;
    }

    void NMaxEvaluator::printPoint(const std::vector<int>& point)
    {
        for (const int i : point)
            std::cout << i << ", ";
    }

}
