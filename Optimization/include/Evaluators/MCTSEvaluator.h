#pragma once
#include <Evaluators/Evaluator.h>
#include <VectorSearchSpace.h>
#include <Stratega/Agent/TreeSearchAgents/MCTSAgent.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

#include <Evaluators/Arena.h>

#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
	/// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class MCTSEvaluator : public Evaluator
    {
    private:
        std::vector<float> k_values;
        std::vector<int> rollout_length;
        std::vector<int> opponent_script;
        
        std::vector<int> currentPoint;

        SGA::GameConfig& config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

        std::unique_ptr<Arena> arena = nullptr;

    public:
        MCTSEvaluator(
            std::vector<float> k_values,
            std::vector<int> rollout_length,
            std::vector<int> opponent_script,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
        //float evaluateGame(std::vector<int> point, int opponentID, bool playFirst);
    	
        std::string printPoint(const std::vector<int>& point) override;

        std::vector<std::unique_ptr<Agent>> generateAgents();
    };

    
}
