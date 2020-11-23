#include <random>
#include <Optimization/Mutator.h>

namespace SGA
{


    Mutator::Mutator(SearchSpace searchSpace, bool swapMutate, bool randomChaosMutate, float mutationPointProbability, bool flipAtLeastOne) :
        _searchSpace(searchSpace),
        _swapMutate(swapMutate),
        _randomChaosMutate(randomChaosMutate),
        _mutationPointProbability(mutationPointProbability),
        _flipAtLeastOne(flipAtLeastOne)
	{
       
    }

    std::vector<int> Mutator::swapMutation(std::vector<int> point, std::mt19937& randomGenerator)
    {
        size_t length = point.size();

        std::vector<int> samples;
        std::sample(point.begin(), point.end(), std::back_inserter(samples), 2, randomGenerator);

        const float a = point[samples[0]];
        const float b = point[samples[1]];

        point[samples[0]] = b;
        point[samples[1]] = a;

        return point;
    }
        

    /// <summary>
    /// mutate the value of x at the given dimension 'dim'
    /// </summary>
    void Mutator::mutateValue(std::vector<int> point, int dim, std::mt19937& randomGenerator)
    {
        point[dim] = _searchSpace.getRandomValueInDim(dim);
    }
	
    std::vector<int> Mutator::mutate(std::vector<int> point, std::mt19937& randomGenerator)
    {
        std::vector<int> new_point = point;
        const size_t length = point.size();

        //Perform swap mutation operation
        if (_swapMutate)
            return swapMutation(point, randomGenerator);

        // Random mutation i.e just return a random search point
        if (_randomChaosMutate)
            return _searchSpace.getRandomPoint();

        const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

        // For each of the dimensions, we mutate it based on mutation_probability
        for (int dim = 0; dim < length; dim++) {
            if (_mutationPointProbability > doubleDistribution_(randomGenerator))
                mutateValue(new_point, dim, randomGenerator);
        }

        // If we want to force flip at least one of the points then we do this here
        if (_flipAtLeastOne)
            mutateValue(new_point, floor(rand() * length), randomGenerator);

        return new_point;
    }
}