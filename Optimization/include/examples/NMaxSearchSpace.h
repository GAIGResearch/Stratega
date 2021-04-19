#pragma once
#include <string>
#include <iostream>
#include <random>

#include <vector>

#include "../SearchSpace.h"


namespace SGA
{

    class NMaxSearchSpace : public SearchSpace
    {

    public:
        int nDims;
        int m;
        std::uniform_int_distribution<> distrib;
    	
        NMaxSearchSpace(int ndims, int m);
        ~NMaxSearchSpace() = default;

        long long getSize() override;
        int getDimSize(int dim) override;

        std::vector<int> getRandomPoint(std::mt19937& randomGenerator) override;
        int getRandomValueInDim(int dim, std::mt19937& randomGenerator) override;

    };

}


