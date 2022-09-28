#pragma once
#include <Stratega/ntbea/Evaluators/Evaluator.h>
#include <Stratega/ntbea/VectorSearchSpace.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSAgent.h>

#include <Stratega/ntbea/Evaluators/Arena.h>

#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class ElasMCTSuEvaluator : public Evaluator
    {
    private:
        std::vector<float> k_values;
        std::vector<int> rollout_length;
        std::vector<float> reward_threshold;
        std::vector<float> transition_threshold;
        std::vector<int> early_stop;
        float fm;
        std::string heuristic;


        std::vector<int> opponent_script;
        
        std::vector<int> currentPoint;

        SGA::GameConfig& config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

        std::unique_ptr<Arena> arena = nullptr;

    public:
        ElasMCTSuEvaluator(
            std::vector<float> k_values,
            std::vector<int> rollout_length,
            std::vector<float> R_T,
            std::vector<float> T_T,
            std::vector<int> early_stop,
            float fm,
            std::string heuristic,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        //float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        void printPoint(const std::vector<int>& point) override;

        std::vector<std::unique_ptr<Agent>> generateAgents();
    };

    
}
