#pragma once
#include <string>
#include <iostream>
#include <set>
#include <numeric>
#include <vector>

#include <Mutator.h>
#include <SearchSpace.h>
#include <Evaluator.h>
#include <NTupleLandscapeModel.h>


namespace SGA
{
	
	class NTBEA
    {
	private:
        BanditLandscapeModel* _tupleLandscape;
        Evaluator* _evaluator;
        SearchSpace* _searchSpace;
        Mutator* _mutator;
        std::mt19937 randomGenerator;

        int _kExplore = 100;
        int _nSamples = 1;
        int _evalNeighbors = 50;
        float _tieBreakNoise = 1e-6;
		
    public:
        NTBEA(BanditLandscapeModel* tupleLandscape, Evaluator* evaluator, SearchSpace* searchSpace, Mutator* mutator,
            int kExplore, int nSamples, int evalNeighbors, int seed = 1);
        virtual ~NTBEA() = default;
		
        virtual void run(int n_evaluations, std::mt19937& randomGenerator);
        virtual std::vector<int> evaluateLandscape(std::vector<int>& point, std::mt19937& randomGenerator);

        [[nodiscard]] std::string getName() const { return "NTBEA"; };
    };
    
}
