#pragma once
#include <Evaluators/Evaluator.h>
#include <VectorSearchSpace.h>
#include <Configuration/GameConfig.h>
#include <Agent/RHEAAgent/RHEAAgent.h>

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
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

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
        float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        void printPoint(const std::vector<int>& point) override;
        
    };
    
}