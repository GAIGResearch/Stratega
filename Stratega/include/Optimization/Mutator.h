#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "SearchSpace.h"

namespace SGA
{
    class Mutator
    {
    private:
        SearchSpace _searchSpace;
        bool _swapMutate;
        bool _randomChaosMutate;
        float _mutationPointProbability;
        bool _flipAtLeastOne;
        std::mt19937 _randomGenerator;
    	
    public:
        Mutator(SearchSpace searchSpace, bool swapMutate, bool randomChaosMutate, float mutationPointProbability, bool flipAtLeastOne);
        std::vector<int> mutate(std::vector<int> point, std::mt19937& randomGenerator);

        [[nodiscard]] std::string getName() const { return "Mutator"; };
    	

	private:
        std::vector<int> swapMutation(std::vector<int> point, std::mt19937& randomGenerator);
        void mutateValue(std::vector<int> point, int dim, std::mt19937& randomGenerator);

    };
}
