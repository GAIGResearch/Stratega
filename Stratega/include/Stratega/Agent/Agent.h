#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <random>

namespace SGA
{
	class Agent
	{
	public:

		//Constructors and destructors
		Agent();
		virtual ~Agent() = default;
		Agent(const Agent& other) = delete;
		Agent(Agent&& other) noexcept = delete;
		Agent& operator=(const Agent& other) = delete;
		Agent& operator=(Agent&& other) noexcept = delete;
		
		/// <summary>
		/// Function for initializing the agent. Override this function to receive a call just before starts.
		/// </summary>
		/// <param name="initialState">Copy of the initial state of the game.</param>
		/// <param name="forwardModel">Forward Model of the game.</param>
		/// <param name="timeBudgetMs">Number of milliseconds to return an action</param>
		virtual void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs);

		/// <summary>
		/// Function for deciding the next action to execute. Must be overriden for an agent to work. Returns an ActionAssignment
		///  which specifies what actions will be executed in the game.
		/// </summary>
		/// <param name="state">Copy of the current state of the game</param>
		/// <param name="forwardModel">Forward model of the game.</param>
		/// <param name="timeBudgetMs">Time in milliseconds to return an action</param>
		/// <returns></returns>
		virtual ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) = 0;

		/// <summary>
		/// Returns this player's ID.
		/// </summary>
		/// <returns>This player's ID</returns>
		int getPlayerID();

		/// <summary>
		/// Sets the ID of this plater
		/// </summary>
		/// <param name="playerID">Player ID to set.</param>
		void setPlayerID(int playerID);

		/// <summary>
		/// Sets the seed for the random number generator of this agent.
		/// </summary>
		/// <param name="seed">Seed for the generator</param>
		void setSeed(std::mt19937::result_type seed = 0);


	private:
		/// <summary>
		/// ID of this player
		/// </summary>
		int playerID;

		/// <summary>
		/// Random number generator for this agent.
		/// </summary>
		std::mt19937 rngEngine;


	protected:

		/// <summary>
		/// Returns the random generator of this player. <See cref="SGA::Agent.setSeed()"/> for how to set the seed of this agent.
		/// </summary>
		/// <returns>The random number generator of this agent</returns>
		std::mt19937& getRNGEngine();


		/* AGENT UTILITY FUNCTIONS*/

		/// <summary>
		/// Filters a vector actions returning those actions of a particular action type ID.
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsByTypeID(const std::vector<Action>& actions, int typeID) const;

		/// <summary>
		/// Filters a vector actions returning those actions of a particular action name.
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsByName(const std::vector<Action>& actions, std::string name) const;

		/// <summary>
		/// Filters a vector actions returning those actions of a particular Action Flag (e.g. EndTick, Continuous, Abort).
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsByFlag(const std::vector<Action>& actions, ActionFlag flag) const;

		/// <summary>
		/// Filters a vector actions returning those actions of a particular Source Type (e.g. Entity, Player).
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsBySource(const std::vector<Action>& actions, ActionSourceType source) const;

		/// <summary>
		/// Filters a vector actions returning those actions that would be executed by a particular entity, whose ID is provided.
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsByEntityID(const std::vector<Action>& actions, int entityID) const;

		/// <summary>
		/// Filters a vector actions returning those actions that would be executed by a particular player, whose ID is provided
		/// </summary>
		/// <param name="actions">Larger vector of actions to filter from.</param>
		/// <param name="typeID">Value of the parameter to use for the filter</param>
		/// <returns>A subset of actions (can be emtpy) that match the seach criteria.</returns>
		std::vector<Action> filterActionsByPlayerID(const std::vector<Action>& actions, int playerID) const;

	};

	
}

