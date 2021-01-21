#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/Heuristic/SimpleHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>

#include <Agent/ActionScripts/BaseActionScript.h>
#include <Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Agent/ActionScripts/RunAwayFromOpponentScript.h>
#include <Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Agent/ActionScripts/RandomActionScript.h>
#include <Agent/PortfolioGreedySearchParams.h>


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

		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		void Improve(TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<TBSUnit*>& playerUnits, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios);

		double Playout(TBSForwardModel& forwardModel, TBSGameState gameState, std::map<int, BaseActionScript*>& playerPortfolios, std::map<int, BaseActionScript*>& opponentPortfolios);

		void InitializePortfolios(std::vector<TBSUnit*>& units, std::map<int, BaseActionScript*>& portfolioMap);
		
		TBSAction GetPortfolioAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, std::map<int, BaseActionScript*>& portfolioMap1, std::map<int, BaseActionScript*>& portfolioMap2);

		void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, const TBSAction& action);
	};
}
