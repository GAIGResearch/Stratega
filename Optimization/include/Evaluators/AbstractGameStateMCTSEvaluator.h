#pragma once
#include <Evaluators/Evaluator.h>
#include <VectorSearchSpace.h>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>

#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{

	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class AbstractGameStateMCTSEvaluator : public Evaluator
    {
    private:
        std::vector<bool> insertMap;
        std::vector<bool> insertPositions;
        std::vector<std::string> parameterNames;

        const SGA::GameConfig* config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

    public:
        AbstractGameStateMCTSEvaluator(
            std::vector<bool> inserMap,
            std::vector<bool> insertPositions,
            GameConfig* config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        void printPoint(const std::vector<int>& point) override;
        
    };
    
}
