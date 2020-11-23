#pragma once
#include <string>

#include <Optimization/SearchSpace.h>
#include <vector>


namespace SGA
{
    class RHEASearchSpace : SearchSpace
    {
    	
    private:

        int _popSizeNrOfOptions;
        int _individualLengthNrOfOptions;
        int _mutationRateNrOfOptions;
        int _tournamentSizeNrOfOptions;
        size_t _searchSpaceSize;

    public:
        RHEASearchSpace(
            int popSizeNrOfOptions,
            int individualLengthNrOfOptions,
            int mutationRateNrOfOptions,
            int tournamentSizeNrOfOptions);
    	~RHEASearchSpace() = default;
    	
        std::vector<int> getRandomPoint() override;
        int getSize() override;
        int getDimSize(int dim) override;
        int getRandomValueInDim(int dim) override;
    };
}

