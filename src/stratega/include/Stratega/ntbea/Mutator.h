#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <random>
#include <boost/random.hpp>

#include <Stratega/ntbea/SearchSpace.h>

namespace SGA
{
	
    class Mutator
    {
    private:
        SearchSpace* _searchSpace;
        bool _swapMutate;
        bool _randomChaosMutate;
        float _mutationPointProbability;
        bool _flipAtLeastOne;
        std::mt19937 _randomGenerator;
        std::vector<int> _indexVector;
    	
    public:
        Mutator(SearchSpace* searchSpace, bool swapMutate, bool randomChaosMutate, float mutationPointProbability, bool flipAtLeastOne);
        std::vector<int> mutate(std::vector<int> point, std::mt19937& randomGenerator) const;

        [[nodiscard]] std::string getName() const { return "Mutator"; };
    	

	private:
        std::vector<int> swapMutation(std::vector<int>& point, std::mt19937& randomGenerator) const;
        void mutateValue(std::vector<int>& point, const int dim, std::mt19937& randomGenerator) const;

    };
    
}
