#pragma once
#include <vector>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/PortfolioForwardModel.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>


namespace SGA {
	struct AgentParameters {

	protected:

		// the script the opponent is simulated with
		// never set this to be a nullptr, use SkipTurnScript instead
		std::shared_ptr<BaseActionScript> opponentModel;
		std::unique_ptr<StateFactory> STATE_FACTORY = nullptr;

		
	public: 

		// agent parameters
		int MAX_FM_CALLS = 2000;					// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game

		std::shared_ptr<StateHeuristic> heuristic;

		// the portfolio used to sample actions of a genome
		// if empty the original forwardModel will be used to generate actions
		std::vector<std::shared_ptr<BaseActionScript>> portfolio;

		AgentParameters()
		{
			//std::shared_ptr<BaseActionScript> random = std::make_shared<RandomActionScript>();
			//portfolio.emplace_back(random);
			portfolio = std::vector<std::shared_ptr<BaseActionScript>>();
			opponentModel = std::make_shared<RandomActionScript>();
			heuristic = std::make_unique<MinimizeDistanceHeuristic>();
		};

		void printDetails() const;

		/// <summary>
		/// Returns a forward model of the given type (RTS/TBS) and portfolio (True/False).
		/// </summary>
		/// <param name="forwardModel"></param>
		/// <returns></returns>
		//std::unique_ptr<ForwardModel> preprocessForwardModel(const TBSForwardModel& forwardModel)
		std::unique_ptr<ForwardModel> preprocessForwardModel(const ForwardModel& forwardModel)
		{
			bool isPortfolio = !portfolio.empty();

			if (forwardModel.getGameType() == SGA::GameType::TBS)
				if (isPortfolio)	return std::make_unique<PortfolioTBSForwardModel>(dynamic_cast<const TBSForwardModel&>(forwardModel), portfolio);
				else				return std::make_unique<TBSForwardModel>(dynamic_cast<const TBSForwardModel&>(forwardModel));

			else if (forwardModel.getGameType() == SGA::GameType::RTS)
				if (isPortfolio)	return std::make_unique<PortfolioRTSForwardModel>(dynamic_cast<const RTSForwardModel&>(forwardModel), portfolio);
				else				return std::make_unique<RTSForwardModel>(dynamic_cast<const RTSForwardModel&>(forwardModel));

			else throw std::exception("Unrecognized forward model type in Agent Parameters.");
		}

		std::shared_ptr<BaseActionScript> AgentParameters::getOpponentModel() const { return opponentModel; }
		std::shared_ptr<StateHeuristic> AgentParameters::getStateHeuristic() const { return heuristic; }
		std::vector<std::shared_ptr<BaseActionScript>> AgentParameters::getPortfolio() const { return portfolio; }

	};
}
