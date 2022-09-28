#pragma once
#include <string>
#include <iostream>
#include <boost/random.hpp>

#include <Stratega/ntbea/SearchSpace.h>
#include <vector>
#include <random>


namespace SGA
{
	
    class VectorSearchSpace : public SearchSpace
    {

    private:

        std::vector<int> _valuesPerDimension;
        size_t _searchSpaceSize = 0;


    public:
        VectorSearchSpace(std::vector<int>& valuesPerDimension);
        ~VectorSearchSpace() = default;

        long long getSize() override;
        int getDimSize(int dim) override;
    	
        std::vector<int> getRandomPoint(std::mt19937& randomGenerator) override;
        int getRandomValueInDim(int dim, std::mt19937& randomGenerator) override;

    };
	
}

