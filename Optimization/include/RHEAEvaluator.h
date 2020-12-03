#pragma once
#include <Evaluator.h>
#include <VectorSearchSpace.h>
#include <Configuration/GameConfig.h>

namespace SGA
{

	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class RHEAEvaluator : public Evaluator
    {
    private:
        std::vector<int> _popSizeCandidates;
        std::vector<int> _individualLengthCandidates;
        std::vector<float> _mutationRateCandidates;
        std::vector<int> _tournamentSize;
        std::vector<bool> _elitism;
        std::vector<bool> _continueSearch;
        const SGA::GameConfig* config;

    public:
        RHEAEvaluator(std::vector<int> popSizeCandidates,
            std::vector<int> individualLengthCandidates,
            std::vector<float> mutationRateCandidates,
            std::vector<int> tournamentSize,
            std::vector<bool> elitism,
            std::vector<bool> continueSearch,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        void printPoint(const std::vector<int>& point) override;
        
    };
    
}