#include <iostream>
#include <Optimization/RHEASearchSpace.h>

namespace SGA
{
	size_t POP_SIZE = 10;				// population size
	size_t INDIVIDUAL_LENGTH = 10;		// planning horizon of an individual

	// evolution and selection
	double MUTATION_RATE = 0.1;			// mutation rate when transferring one individual to the next generation
	int TOURNAMENT_SIZE = 3;			// number of individuals per tournament selection

	RHEASearchSpace::RHEASearchSpace(
		int popSizeNrOfOptions,
		int individualLengthNrOfOptions, 
		int mutationRateNrOfOptions,
		int tournamentSizeNrOfOptions)
	: SearchSpace("RHEASearchSpace", 4),
	_popSizeNrOfOptions(popSizeNrOfOptions), _individualLengthNrOfOptions(individualLengthNrOfOptions),
	_mutationRateNrOfOptions(mutationRateNrOfOptions), _tournamentSizeNrOfOptions(tournamentSizeNrOfOptions)
	{
		_searchSpaceSize = _popSizeNrOfOptions * _individualLengthNrOfOptions *
			_mutationRateNrOfOptions * _tournamentSizeNrOfOptions;
	}

	std::vector<int> RHEASearchSpace::getRandomPoint()
	{
		return std::vector{ rand() % _popSizeNrOfOptions, rand() % _individualLengthNrOfOptions,
			rand() % _mutationRateNrOfOptions, rand() % _tournamentSizeNrOfOptions };
	}
	
	int RHEASearchSpace::getSize()
	{
		return _searchSpaceSize;
	}
	
	int RHEASearchSpace::getDimSize(int dim)
	{
		switch (dim)
		{
			case 0: return _popSizeNrOfOptions;
			case 1: return _individualLengthNrOfOptions;
			case 2: return _mutationRateNrOfOptions;
			case 3: return _tournamentSizeNrOfOptions;
			default:
				std::cout << "dimension " << dim << " unknown to RHEASearchSpace" << std::endl;
				throw 1;
		}
	}
	
	int RHEASearchSpace::getRandomValueInDim(int dim)
	{
		switch (dim)
		{
			case 0: return rand() % _popSizeNrOfOptions;
			case 1: return rand() % _individualLengthNrOfOptions;
			case 2: return rand() % _mutationRateNrOfOptions;
			case 3: return rand() % _tournamentSizeNrOfOptions;
			default:
				//std::snprintf("dimension %d unknown to RHEASearchSpace", dim)
				throw std::exception();
		}
	}
	
}
