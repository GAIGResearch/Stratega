#pragma once
#include <string>
#include <iostream>

#include <SearchSpace.h>
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

        size_t POP_SIZE = 10;				// population size
        size_t INDIVIDUAL_LENGTH = 10;		// planning horizon of an individual

        // evolution and selection
        double MUTATION_RATE = 0.1;			// mutation rate when transferring one individual to the next generation
        int TOURNAMENT_SIZE = 3;			// number of individuals per tournament selection


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

