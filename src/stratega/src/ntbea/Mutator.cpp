#include <Stratega/ntbea/Mutator.h>
#include <algorithm>
//#include <iterator>

namespace SGA
{
    
    Mutator::Mutator(SearchSpace* searchSpace, bool swapMutate, bool randomChaosMutate, float mutationPointProbability, bool flipAtLeastOne) :
        _searchSpace(searchSpace),
        _swapMutate(swapMutate),
        _randomChaosMutate(randomChaosMutate),
        _mutationPointProbability(mutationPointProbability),
        _flipAtLeastOne(flipAtLeastOne)
	{
        for (int i = 0; i < _searchSpace->getNumDims(); i++)
            _indexVector.emplace_back(i);
    }
	
    
    std::vector<int> Mutator::swapMutation(std::vector<int>& point, std::mt19937& randomGenerator) const
    {
        size_t length = point.size();

        std::vector<int> samples;
        std::sample(_indexVector.begin(), _indexVector.end(), std::back_inserter(samples), 2, randomGenerator);

        const float a = point[samples[0]];
        const float b = point[samples[1]];

        point[samples[0]] = b;
        point[samples[1]] = a;

        return point;
    }
        
    /// <summary>
    /// mutate the value of x at the given dimension 'dim'
    /// </summary>
    void Mutator::mutateValue(std::vector<int>& point, const int dim, std::mt19937& randomGenerator) const
    {
        point[dim] = _searchSpace->getRandomValueInDim(dim, randomGenerator);
    }
	
    std::vector<int> Mutator::mutate(std::vector<int> point, std::mt19937& randomGenerator) const
    {
        std::vector<int> new_point = point;
        const size_t length = point.size();

        //Perform swap mutation operation
        if (_swapMutate)
            return swapMutation(point, randomGenerator);

        // Random mutation i.e just return a random search point
        if (_randomChaosMutate)
            return _searchSpace->getRandomPoint(randomGenerator);

        //const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
        boost::random::uniform_real_distribution<double> doubleDistribution_ = boost::random::uniform_real_distribution<double>(0, 1);

        // For each of the dimensions, we mutate it based on mutation_probability
        for (size_t dim = 0; dim < length; dim++) {
            if (_mutationPointProbability > doubleDistribution_(randomGenerator))
                mutateValue(new_point, dim, randomGenerator);
        }

        // If we want to force flip at least one of the points then we do this here
        if (_flipAtLeastOne)
        {
            //std::uniform_int_distribution<> distrib(0, _searchSpace->getNumDims()-1);
            boost::random::uniform_int_distribution<size_t> distrib(0, _searchSpace->getNumDims()-1);
            mutateValue(new_point, distrib(randomGenerator), randomGenerator);
        }

        return new_point;
    }
	
}