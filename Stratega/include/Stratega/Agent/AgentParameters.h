#pragma once
#include <vector>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/ForwardModel/PortfolioTBSForwardModel.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>


namespace SGA {
	struct AgentParameters {
		
		// agent parameters
		int MAX_FM_CALLS = 2000;				// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game

		// the script the opponent is simulated with
		// never set this to be a nullptr, use SkipTurnScript instead
		std::unique_ptr<BaseActionScript> OPPONENT_MODEL = std::make_unique<RandomActionScript>();	
		std::unique_ptr<StateHeuristic> OBJECTIVE = std::make_unique<MinimizeDistanceHeuristic>();
		std::unique_ptr<StateFactory> STATE_FACTORY = nullptr;

		// the portfolio used to sample actions of a genome
		// if empty the original forwardModel will be used to generate actions
		std::vector<std::unique_ptr<BaseActionScript>> PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();	

		AgentParameters()
		{
			std::unique_ptr<BaseActionScript> random = std::make_unique<RandomActionScript>();
			PORTFOLIO.emplace_back(std::move(random));
		};

		void printDetails() const;

		/// <summary>
		/// Creates an abstract forward model depending on if a portfolio has been set.
		/// </summary>
		/// <param name="forwardModel"></param>
		/// <returns></returns>
		std::unique_ptr<TBSForwardModel> preprocessForwardModel(const TBSForwardModel& forwardModel)
		{
			if (PORTFOLIO.empty())
			{
				return std::make_unique<TBSForwardModel>(forwardModel);
			}
			else
			{
				return std::make_unique<PortfolioTBSForwardModel>(forwardModel, PORTFOLIO);
			}
		}

	};
}
