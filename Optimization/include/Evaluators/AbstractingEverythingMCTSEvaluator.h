#pragma once
#include <Evaluators/Evaluator.h>
#include <VectorSearchSpace.h>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>
#include <Evaluators/Arena.h>

#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class AbstractingEverythingMCTSEvaluator : public Evaluator
    {
    private:
        std::vector<bool> insertMap;
        std::vector<bool> insertPositions;
        std::vector<std::string> parameterNames;
        std::vector<int> currentPoint;

        SGA::GameConfig& config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

        std::unique_ptr<Arena> arena = nullptr;

    public:
        AbstractingEverythingMCTSEvaluator(
            std::vector<bool> inserMap,
            std::vector<bool> insertPositions,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        //float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        void printPoint(const std::vector<int>& point) override;

        std::vector<std::unique_ptr<Agent>> generateAgents();
    };

    
}
