#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

#include <Stratega/Agent/PortfolioGreedySearchParams.h>


namespace SGA
{
	
		
	class PortfolioGreedySearchAgent : public Agent
	{
	private:
		PortfolioGreedySearchParams params_;
		
	public:
		PortfolioGreedySearchAgent(PortfolioGreedySearchParams&& params) :
			Agent{}, params_(std::move(params))
		{}

		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;

		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;

	private:
		void Improve(const ForwardModel& forwardModel, GameState& gameState, std::map<int, BaseActionScript*>& unitScriptAssignment, int playerID);

		double Playout(const ForwardModel& forwardModel, GameState gameState, std::map<int, BaseActionScript*>& unitScriptAssignments, int playerID);

		void InitializePortfolios(std::map<int, std::vector<Entity*>>& unitsPerPlayer, std::map<int, BaseActionScript*>& portfolioMap);
		
		Action GetPortfolioAction(GameState& gameState, std::vector<SGA::Action>& actionSpace, const std::map<int, BaseActionScript*>& unitScriptAssignments);

		void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action, std::map<int, BaseActionScript*>& unitScriptAssignments, int playerID);

		inline int roll(GameState& gs, const ForwardModel& fm, const Action& act, int playerID, const std::map<int, BaseActionScript*>& unitScriptAssignments);
		
		inline int rollOppOnly(GameState& gs, const ForwardModel& fm, const std::map<int, BaseActionScript*>& unitScriptAssignments);

		inline bool rollOppAction(GameState& gs, const ForwardModel& fm, const std::map<int, BaseActionScript*>& unitScriptAssignments, int oppID);

	};


}
