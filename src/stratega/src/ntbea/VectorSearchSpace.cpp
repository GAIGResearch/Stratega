#include <Stratega/ntbea/VectorSearchSpace.h>

namespace SGA
{
	
	VectorSearchSpace::VectorSearchSpace(std::vector<int>& valuesPerDimension)
	: SearchSpace("VectorSearchSpace", valuesPerDimension.size()),
	_valuesPerDimension(std::move(valuesPerDimension))
	{
		if (!_valuesPerDimension.empty())
		{
			_searchSpaceSize = _valuesPerDimension[0];
			for (int i = 1; i < _valuesPerDimension.size(); i++)
			{
				_searchSpaceSize *= _valuesPerDimension[i];
			}
		}
	}

	
	std::vector<int> VectorSearchSpace::getRandomPoint(std::mt19937& randomGenerator)
	{
		std::vector<int> newPoint;
		for (int dim = 0; dim < _valuesPerDimension.size(); dim++)
		{
			newPoint.emplace_back(getRandomValueInDim(dim, randomGenerator));
		}
		return newPoint;
	}
	
	long long VectorSearchSpace::getSize()
	{
		return _searchSpaceSize;
	}
	
	int VectorSearchSpace::getDimSize(int dim)
	{
		return _valuesPerDimension[dim];
	}
	
	int VectorSearchSpace::getRandomValueInDim(int dim, std::mt19937& randomGenerator)
	{
		//const std::uniform_int_distribution<> distribution(0, _valuesPerDimension[dim]-1);
        boost::random::uniform_int_distribution<size_t> distribution(0, _valuesPerDimension[dim]-1);
		return distribution(randomGenerator);
	}
	
}
