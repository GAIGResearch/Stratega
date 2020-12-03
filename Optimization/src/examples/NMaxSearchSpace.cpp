#include "examples/NMaxSearchSpace.h"


namespace SGA
{

	NMaxSearchSpace::NMaxSearchSpace(
		int nDims,
		int m)
		: SearchSpace("NMaxSearchSpace", nDims),
		nDims(nDims), m(m), distrib(0, m-1)
	{
	}


	std::vector<int> NMaxSearchSpace::getRandomPoint(std::mt19937& randomGenerator)
	{
		std::vector<int> point;
		for (int i = 0; i < nDims; i++)
		{
			point.emplace_back(getRandomValueInDim(i, randomGenerator));
		}
		return point;
	}

	long long NMaxSearchSpace::getSize()
	{
		return std::pow(m, nDims);
	}

	int NMaxSearchSpace::getDimSize(int dim)
	{
		return m;
	}

	int NMaxSearchSpace::getRandomValueInDim(int dim, std::mt19937& randomGenerator)
	{
		return distrib(randomGenerator);
	}

}