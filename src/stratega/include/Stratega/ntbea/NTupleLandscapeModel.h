#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include <Stratega/ntbea/BanditLandscapeModel.h>
#include <Stratega/ntbea/SearchSpace.h>

namespace SGA
{
	
	struct TupleStats
	{
        int n = 0;
        double min = 0.0f;
        double max = 0.0f;
        double sum = 0.0f;
        double sum_squared = 0.0f;
	};
	
    class NTupleLandscapeModel : public BanditLandscapeModel
	{
    private:
        int _nDims;
        std::vector<std::vector<int>> _tuples;
        std::vector<int> _tupleConfig;
        std::map<std::vector<int>, std::map<std::vector<int>, TupleStats>> _tupleStats = std::map<std::vector<int>, std::map<std::vector<int>, TupleStats>>();
        std::map<std::vector<int>, TupleStats> _tupleTotalStats = std::map<std::vector<int>, TupleStats>();
    	
        float _ucbEpsilon;
        std::set<std::vector<int>> _sampledPoints;
			
    public:
        NTupleLandscapeModel(SearchSpace* searchSpace, std::vector<int> tuple_config = {}, float ucbEpsilon = 0.5);
        ~NTupleLandscapeModel() = default;

    	
        void reset() override;
        void init() override;
        void addEvaluatedPoint(std::vector<int>& point, const double fitness) override;
        double getMeanEstimate(const std::vector<int>& point) override;
        double getExplorationEstimate(std::vector<int>& point) override;

        static std::vector<std::vector<int>> getTupleCombinations(const int r, const int nDims);
        static std::vector<std::vector<int>> getUniqueCombinations(const size_t idx, const int r, std::vector<int> sourceArray);
        std::pair<std::vector<int>, double> getBestSampled() override;
        
    };
	
}


