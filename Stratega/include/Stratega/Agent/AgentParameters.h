#pragma once
#include <vector>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/PortfolioForwardModel.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#include <Stratega/Utils/Timer.h>


namespace SGA {

	enum class Budget
	{
		TIME, 
		ITERATIONS, 
		FMCALLS,
		UNDEFINED
	};

	struct AgentParameters {

	protected:

		// the script the opponent is simulated with
		// never set this to be a nullptr, use SkipTurnScript instead
		std::shared_ptr<BaseActionScript> opponentModel;
		std::unique_ptr<StateFactory> STATE_FACTORY = nullptr;

		
	public: 

		// agent parameters


		/// <summary>
		/// The maximum number of forward model calls. To be used if this->budgetType == FMCALLS;
		/// </summary>
		int maxFMCalls = 100;					

		/// <summary>
		/// Maximum number of iterations for a decision making round. Applies when this->budgetType == ITERATIONS;
		/// </summary>
		int maxIterations = 10;

		/// <summary>
		/// Current number of FM calls remaining in this cycle. To be updated by the agent.
		/// </summary>
		int currentFMCalls;

		/// <summary>
		/// Current number of iterations executed. To be updated and initialized by the agent.
		/// </summary>
		int currentIterations;

		/// <summary>
		/// ID of this player.
		/// </summary>
		int PLAYER_ID = -1;

		/// <summary>
		/// Type of budget for the agent's decision making.
		/// </summary>
		Budget budgetType;

		/// <summary>
		/// Heuristic to evaluate states.
		/// </summary>
		std::shared_ptr<StateHeuristic> heuristic;

		/// <summary>
		/// This is the portfolio used to sample actions 
		/// if empty the original forwardModel will be used to generate actions
		/// </summary>
		std::vector<std::shared_ptr<BaseActionScript>> portfolio;

		/// <summary>
		/// Timer for action decision, as received by the game. Must be updated by agent at each step.
		/// </summary>
		Timer timer;

		AgentParameters()
		{
			//std::shared_ptr<BaseActionScript> random = std::make_shared<RandomActionScript>();
			//portfolio.emplace_back(random);
			//heuristic = std::make_unique<MinimizeDistanceHeuristic>();
			portfolio = std::vector<std::shared_ptr<BaseActionScript>>();
			opponentModel = std::make_shared<RandomActionScript>();
			budgetType = Budget::UNDEFINED;
		};

		void initBudget(const Timer& timer)
		{
			this->timer = timer;
			currentFMCalls = 0;
			currentIterations = 0;
		}

		bool isBudgetOver() const 
		{
			switch (budgetType)
			{
			case Budget::TIME:
				return timer.exceededMaxTime();
			case Budget::ITERATIONS:
				return currentIterations >= maxIterations;
			case Budget::FMCALLS:
				return currentFMCalls >= maxFMCalls;
			}
			return false;

		}

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

		std::shared_ptr<BaseActionScript> getOpponentModel() const { return opponentModel; }
		std::shared_ptr<StateHeuristic> getStateHeuristic() const { return heuristic; }
		std::vector<std::shared_ptr<BaseActionScript>> getPortfolio() const { return portfolio; }

	};
}
