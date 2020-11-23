#include <iostream>
#include <Optimization/NTupleLandscapeModel.h>


namespace SGA
{
    NTupleLandscapeModel::NTupleLandscapeModel(SearchSpace searchSpace, std::vector<int> tupleConfig, float ucbEpsilon) :
        BanditLandscapeModel("NTupleLandscapeModel"),
		_tupleConfig(tupleConfig),
        _ucbEpsilon(ucbEpsilon)
    {
        _tuples = std::vector<std::vector<int>>();
        _nDims = searchSpace.getNumDims();

        reset();
    }

    void NTupleLandscapeModel::reset()
    {
        _tupleStats.clear();
    }

    /// <summary>
    /// Get the unique combinations of tuples for the n - tuple landscape
    ///     the 'n' value of this tuple
    ///     the number of dimensions in the search space
    /// 
    /// </summary>
    std::vector<std::vector<int>> NTupleLandscapeModel::getTupleCombinations(int r, int nDims)
    {
        std::vector<int> sourceArray = std::vector<int>();
        for (int i = 0; i < nDims; i++)
            sourceArray.emplace_back(i);
        return getUniqueCombinations(0, r, sourceArray);
    }

    std::vector<std::vector<int>> NTupleLandscapeModel::getUniqueCombinations(int idx, int r, std::vector<int> sourceArray)
    {
        std::vector<std::vector<int>> result = std::vector<std::vector<int>>();
    	
        for (size_t i = 0; i < sourceArray.size(); i++)
        {
            if ((r - 1) > 0)
            {
                auto next_level = getUniqueCombinations(i + 1, r - 1, sourceArray);
                for (auto x : next_level)
                {
                    std::vector<int> value = {sourceArray[i]};
                    value.emplace_back(x);
                    result.emplace_back(value);
            	}
            }
            else
            {
                result.emplace_back(sourceArray[i]);
            }
        }
        
		return result;
    }

    /// Create the index combinations for each of the n-tuples
    void NTupleLandscapeModel::init()
    {
        // Create all possible tuples for each n-tuples
        for (const int n : _tupleConfig) 
        {
            std::vector<std::vector<int>> n_tuples = getTupleCombinations(n, _nDims);
            _tuples.insert(n_tuples.end(), n_tuples.begin(), n_tuples.end());

            std::cout << "Added " << n << "-tuples: " << std::endl;

            for (auto i = n_tuples.begin(); i != n_tuples.end(); ++i)
            {
                std::cout << "\t";
                for (auto j = (*i).begin(); j != (*i).end(); ++i)
                {
                    std::cout << *j << ", ";
                }
                std::cout << std::endl;
            }
        }

        std::cout << "Tuple Landscape Size: " << _tuples.size() << std::endl;
    }

    /// Add a point and it's fitness to the tuple landscape
    void NTupleLandscapeModel::addEvaluatedPoint(std::vector<int> point, double fitness)
    {
        _sampledPoints.insert(point);

        for (const auto& tup : _tuples)
        {
            // The search space value is the values given by applying the tuple to the search space.
            // This is used to index the stats at that point for the particular tuple in question
            std::vector<int> searchSpaceValue;
            for (int& i : tup)
                searchSpaceValue.emplace_back(point[i]);

            // Use 'totals' as a key to store summary data of the tuple
        	// todo initialize in case the TupleStat does not exist
            _tupleTotalStats[tup].n += 1;

            // todo initialize in case the TupleStat does not exist
            TupleStats& searchSpaceTupleStats = _tupleStats[tup][searchSpaceValue];
            searchSpaceTupleStats.n += 1;
            searchSpaceTupleStats.max = std::max(searchSpaceTupleStats.max, fitness);
            searchSpaceTupleStats.min = std::min(searchSpaceTupleStats.min, fitness);
            searchSpaceTupleStats.sum += fitness;
            searchSpaceTupleStats.sum_squared += std::pow(fitness, 2);
        }
    }

    /// <summary>
    /// Iterate over all the tuple stats we have stored for this point and sum the means and the number of stats we have found.
    /// Finally the sum of the means divided by the total number of stats found is returned
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    double NTupleLandscapeModel::getMeanEstimate(std::vector<int> point)
    {
        double sum = 0;
        int tuple_count = 0;
        for (std::tuple tup : _tuples)
        {
            std::vector<int> searchSpaceValue;
            for (int& i : tup)
            {
                searchSpaceValue.emplace_back(point[i]);
            }

        	TupleStats tupleStats = _tupleStats[tup][searchSpaceValue];
            if (tupleStats.n > 0)
            {
                sum += tupleStats.sum / tupleStats.n;
                tuple_count++;
            }
        }

        if (tuple_count == 0)
            return 0;
        return sum / tuple_count;
    }

    /// Calculate the average of the exploration across all tuples of the exploration
    double NTupleLandscapeModel::getExplorationEstimate(std::vector<int> point)
    {
        double sum = 0;
        int tuple_count = _tuples.size();

        for (std::tuple tup : _tuples)
        {
            std::vector<int> searchSpaceValue;
            for (int& i : tup)
                searchSpaceValue.emplace_back(point[i]);

        	auto currentTupleStats = _tupleStats[tup];
            TupleStats searchSpaceTupleStats = currentTupleStats[searchSpaceValue];

            if (searchSpaceTupleStats.n == 0)
            {
                const int n = _tupleTotalStats[tup].n;
                sum += std::sqrt(std::log(1 + n) / _ucbEpsilon);
            }
            else
            {
                const int n = searchSpaceTupleStats.n;
                sum += std::sqrt(std::log(1 + n) / (_ucbEpsilon + n));
            }
        }

        return sum / tuple_count;
    }

    std::pair<std::vector<int>, double> NTupleLandscapeModel::getBestSampled()
    {
        double currentBestMean = 0;
        std::vector<int> currentBestPoint;
        for (const std::vector<int>& point : _sampledPoints)
        {
            const float mean = getMeanEstimate(point);

            if (mean > currentBestMean)
            {
                currentBestMean = mean;
                currentBestPoint = point;
            }
        }
        return std::pair<std::vector<int>, double> {currentBestPoint, currentBestMean};
    }

}

