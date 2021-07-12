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
		void Improve(const ForwardModel& forwardModel, GameState& gameState, std::vector<Entity*>& playerUnits, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios);

		double Playout(const ForwardModel& forwardModel, GameState gameState, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios);

		void InitializePortfolios(std::vector<Entity*>& units, std::map<int, BaseActionScript*>& portfolioMap);
		
		Action GetPortfolioAction(GameState& gameState, std::vector<SGA::Action>& actionSpace, std::map<int, BaseActionScript*>& portfolioMap1, std::map<int, BaseActionScript*>& portfolioMap2);

		void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action);
	};
}
