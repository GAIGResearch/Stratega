#pragma once
#include <vector>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/PortfolioForwardModel.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#include <Stratega/Utils/Timer.h>
#include <yaml-cpp/yaml.h>


namespace SGA {

	/// <summary>
	/// Indicates the type of budget configured for this agent.
	/// </summary>
	enum class Budget
	{
		TIME, 
		ITERATIONS, 
		FMCALLS,
		UNDEFINED
	};

	/// <summary>
	/// Turns the budget received as parameter to a readable string.
	/// </summary>
	/// <param name="type">Budget of a given type.</param>
	/// <returns>A readable string</returns>
	inline std::string budgetTypeStr(Budget type)
	{
		switch (type)
		{
		case Budget::TIME: return "Time";
		case Budget::ITERATIONS: return "Iterations";
		case Budget::FMCALLS: return "Forward Model calls";
		case Budget::UNDEFINED: return "Undefined";
		}
		return "Undefined";
	}

	struct AgentParameters {

	public:

		/// <summary>
		/// The script the opponent is simulated with. Never set this to be a nullptr, use SkipTurnScript instead
		/// </summary>
		std::shared_ptr<BaseActionScript> opponentModel;

		/// <summary>
		/// A factory of abstracted states.
		/// </summary>
		std::unique_ptr<StateFactory> stateFactory = nullptr;

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
		int currentFMCalls = 0;

		/// <summary>
		/// Current number of iterations executed. To be updated and initialized by the agent.
		/// </summary>
		int currentIterations = 0;

		/// <summary>
		/// Proportion of the timer that must have passed before the function isBudgetOver triggers.
		/// </summary>
		double percTimeBudget = 0.9;

        std::string HEURISTIC = "ktk"; // should be {ktk, pta}

		/// <summary>
		/// ID of this player.
		/// </summary>
		int PLAYER_ID = -1;

		/// <summary>
		/// Type of budget for the agent's decision making.
		/// </summary>
		Budget budgetType;

		/// <summary>
		/// This is the portfolio used to sample actions 
		/// if empty the original forwardModel will be used to generate actions
		/// </summary>
		std::vector<std::shared_ptr<BaseActionScript>> portfolio;

		/// <summary>
		/// Heuristic to evaluate states.
		/// </summary>
		std::shared_ptr<StateHeuristic> heuristic;

		/// <summary>
		/// Timer for action decision, as received by the game. Must be updated by agent at each step.
		/// </summary>
		Timer timer;

		/// <summary>
		/// Constructor for agent parameters.
		/// </summary>
		AgentParameters();

		/// <summary>
		/// Initializes the budget for a  decision making step.
		/// </summary>
		/// <param name="timer">The timer as provided by Stratega to the agent.</param>
		void resetCounters(const Timer& timer);

		/// <summary>
		/// Indicates if the budget to return an action is exhausted.
		/// </summary>
		/// <returns></returns>
		bool isBudgetOver() const;

		/// <summary>
		/// Prints the parameters of this agent.
		/// </summary>
		void printDetails() const;

		/// <summary>
		/// Returns a forward model of the given type (RTS/TBS) and portfolio (True/False).
		/// </summary>
		/// <param name="forwardModel"></param>
		/// <returns></returns>
		//std::unique_ptr<ForwardModel> preprocessForwardModel(const TBSForwardModel& forwardModel)
		std::unique_ptr<ForwardModel> preprocessForwardModel(const ForwardModel& forwardModel);

		/// <summary>
		/// Returns the script used as opponent model.
		/// </summary>
		std::shared_ptr<BaseActionScript> getOpponentModel() const { return opponentModel; }

		/// <summary>
		/// Returns the portfolio of scripts this agent has access to.
		/// </summary>
		std::vector<std::shared_ptr<BaseActionScript>> getPortfolio() const { return portfolio; }

		/// <summary>
		/// For initizliing the agent, reads and assign parameters from a YAML Node.
		/// </summary>
		/// <param name="node"></param>
		void decode(const YAML::Node& node);

	};
}

