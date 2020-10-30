#pragma once
#include <vector>

#include <Agent/Heuristic/LinearSumHeuristic.h>

#include <Agent/ActionScripts/BaseActionScript.h>
#include <Agent/ActionScripts/AttackClosestOpponentScript.h>


#include "ActionScripts/AttackWeakestOpponentScript.h"
#include "ActionScripts/RandomActionScript.h"
#include "ActionScripts/RunAwayFromOpponentScript.h"
#include "ActionScripts/RunToFriendlyUnitsScript.h"
#include "ActionScripts/UseSpecialAbilityScript.h"
#include "ForwardModel/PortfolioTBSForwardModel.h"


namespace SGA {
	struct AgentParameters {
		
		// agent parameters
		int MAX_FM_CALLS = 2000;					// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game

		// the script the opponent is simulated with
		// never set this to be a nullptr, use SkipTurnScript instead
		std::unique_ptr<BaseActionScript> OPPONENT_MODEL = std::make_unique<AttackClosestOpponentScript>();	
		std::unique_ptr<StateHeuristic> OBJECTIVE = std::make_unique<LinearSumHeuristic>();

		// the portfolio used to sample actions of a genome
		// if empty the original forwardModel will be used to generate actions
		std::vector<std::unique_ptr<BaseActionScript>> PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();	

		AgentParameters()
		{
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			PORTFOLIO.emplace_back(std::move(attackWeak));
			std::unique_ptr<BaseActionScript> runAway = std::make_unique<RunAwayFromOpponentScript>();
			PORTFOLIO.emplace_back(std::move(runAway));
			std::unique_ptr<BaseActionScript> useSpecialAbility = std::make_unique<UseSpecialAbilityScript>();
			PORTFOLIO.emplace_back(std::move(useSpecialAbility));
			std::unique_ptr<BaseActionScript> runToFriends = std::make_unique<RunToFriendlyUnitsScript>();
			PORTFOLIO.emplace_back(std::move(runToFriends));
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			PORTFOLIO.emplace_back(std::move(random));
		};

		void printDetails() const;

		/// <summary>
		/// Creates an abstract forward model depending on if a portfolio has been set.
		/// </summary>
		/// <param name="forwardModel"></param>
		/// <returns></returns>
		std::unique_ptr<TBSForwardModel> preprocessForwardModel(TBSForwardModel* forwardModel)
		{
			if (PORTFOLIO.empty())
			{
				return std::unique_ptr<TBSForwardModel>(forwardModel);
			}
			else
			{
				return std::make_unique<PortfolioTBSForwardModel>(*forwardModel, PORTFOLIO);
			}
		}

	};
}
