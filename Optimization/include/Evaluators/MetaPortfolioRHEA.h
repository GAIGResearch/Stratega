#pragma once
#include <Evaluators/Evaluator.h>
#include <VectorSearchSpace.h>
#include <Configuration/GameConfig.h>
#include <Agent/MetaPortfolioOptimizer/MetaPOAgent.h>

namespace SGA
{

	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class MetaPortfolioRHEAEvaluator : public Evaluator
    {
    private:
        std::vector<int> _popSizeCandidates;
        std::vector<int> _horizonCandidates;
        std::vector<float> _mutationRateCandidates;
        std::vector<int> _tournamentSize;
        std::vector<bool> _elitism;
        std::vector<bool> _continueSearch;
        std::vector<int> _nrOfChanges;
        std::vector<bool> _p1;
        std::vector<bool> _p2;
        std::vector<bool> _p3;
        std::vector<bool> _p4;
        std::vector<bool> _p5;
        std::vector<bool> _p6;
    	
        const SGA::GameConfig* config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

    public:
        MetaPortfolioRHEAEvaluator(std::vector<int> popSizeCandidates,
            std::vector<int> horizon,
            std::vector<float> mutationRateCandidates,
            std::vector<int> tournamentSize,
            std::vector<bool> elitism,
            std::vector<bool> continueSearch,
            std::vector<int> nrOfChanges,
            std::vector<bool> p1,
            std::vector<bool> p2,
            std::vector<bool> p3,
            std::vector<bool> p4,
            std::vector<bool> p5,
            std::vector<bool> p6,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        void printPoint(const std::vector<int>& point) override;
        
    };
    
}