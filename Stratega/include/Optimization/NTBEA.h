#pragma once
#include <string>
#include <iostream>

#include <Optimization/Mutator.h>
#include <Optimization/SearchSpace.h>
#include <Optimization/Evaluator.h>
#include <Optimization/NTupleLandscapeModel.h>

namespace SGA
{
	class NTBEA
    {
	private:
        SearchSpace* _searchSpace;
        Mutator _mutator;
        NTupleLandscapeModel _tupleLandscape;
        Evaluator* _evaluator;

        int _kExplore = 100;
        int _nSamples = 1;
        int _evalNeighbors = 50;
        float _tieBreakNoise = 1e-6;
		
    public:
        NTBEA(NTupleLandscapeModel& tupleLandscape, Evaluator& evaluator, SearchSpace& searchSpace, Mutator& mutator, 
            int kExplore, int nSamples, int eval_neighbours);

        virtual void run(int n_evaluations);
        virtual std::vector<int> evaluateLandscape(std::vector<int> point);

        [[nodiscard]] std::string getName() const { return "NTBEA"; };
    };
}
