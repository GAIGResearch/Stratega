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
    class ActionAbstractingMCTSEvaluator : public Evaluator
    {
    private:
        std::vector<float> k_values;			        // values of k
        std::vector<int> rollout_length;	            // rollout length
        std::vector<int> opponent_script_index;			// opponent scripts (Attack Closest, Attack Weakest, Random, nullptr=SkipTurn)
        std::vector<bool> attack_closest;				// Attack Closest
        std::vector<bool> attack_weakest;				// Attack Weakest
        std::vector<bool> run_away;				        // Run Away From Opponent
        std::vector<bool> run_to_friend;				// Run to Friendly Unit
        std::vector<bool> special_ability;				// Use Special Ability
        std::vector<bool> random_script;				// Random Script

        std::vector<int> currentPoint;

        SGA::GameConfig& config;
        std::mt19937 rngEngine = std::mt19937(0);
        const std::vector<std::unique_ptr<Agent>> agents;

        std::unique_ptr<Arena> arena = nullptr;

    public:
        ActionAbstractingMCTSEvaluator(
            std::vector<float> k_values,
            std::vector<int> rollout_length,
            std::vector<int> opponent_script_index,
            std::vector<bool> attack_closest,
            std::vector<bool> attack_weakest,
            std::vector<bool> run_away,
            std::vector<bool> run_to_friend,
            std::vector<bool> special_ability,
            std::vector<bool> random_script,
            SGA::GameConfig& config
        );
    
        std::vector<float> evaluate(std::vector<int> point, int nSamples) override;
    	
        std::string printPoint(const std::vector<int>& point) override;

        std::vector<std::unique_ptr<Agent>> generateAgents();
    };

    
}
