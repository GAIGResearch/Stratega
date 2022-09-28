#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Agent/AgentParameters.h>
#include <boost/random.hpp>
#include <Stratega/Utils/Timer.h>
namespace SGA
{
	class Agent
	{
	public:
        bool is_debug=true;

		//Constructors and destructors
		Agent(const std::string& name);
		virtual ~Agent() = default;
		Agent() = delete;
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
		virtual void init(GameState initialState, const ForwardModel& forwardModel, Timer timer);

		/// <summary>
		/// Function for deciding the next action to execute. Must be overriden for an agent to work. Returns an ActionAssignment
		///  which specifies what actions will be executed in the game.
		/// </summary>
		/// <param name="state">Copy of the current state of the game</param>
		/// <param name="forwardModel">Forward model of the game.</param>
		/// <param name="timeBudgetMs">Time in milliseconds to return an action</param>
		/// <returns></returns>
		virtual ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) = 0;

		/// <summary>
		/// Returns this player's ID.
		/// </summary>
		/// <returns>This player's ID</returns>
		virtual int getPlayerID();

		/// <summary>
		/// Sets the ID of this plater
		/// </summary>
		/// <param name="playerID">Player ID to set.</param>
		virtual void setPlayerID(int playerID);

		/// <summary>
		/// Sets the seed for the random number generator of this agent.
		/// </summary>
		/// <param name="seed">Seed for the generator</param>
		virtual void setSeed(boost::mt19937::result_type seed = 0);

		/// <summary>
		/// Returns the name of this agent.
		/// </summary>
		virtual const std::string& getName() const { return agentName; }


	private:
		/// <summary>
		/// ID of this player
		/// </summary>
		int playerID;

		/// <summary>
		/// Random number generator for this agent.
		/// </summary>
		boost::mt19937 rngEngine;




	protected:

		/// <summary>
		/// Returns the random generator of this player. <See cref="SGA::Agent.setSeed()"/> for how to set the seed of this agent.
		/// </summary>
		/// <returns>The random number generator of this agent</returns>
		boost::mt19937& getRNGEngine();

		/// <summary>
		/// Name of this agent. Useful for bindings.
		/// </summary>
		std::string agentName = "DefaultAgent";


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


	/// <summary>
	/// Given an opponent ID, it rolls the state forward executing an action dictated by the opponent model from agParams.
	/// If no opponent model is set, nothing / skip turn is executed.
	/// </summary>
	/// <param name="gs">Game state to roll forward</param>
	/// <param name="fm">Forward model to roll the state forward and quering available actions.</param>
	/// <param name="agParams">Parameters of the agent who's calling this method.</param>
	/// <param name="oppID">ID of the opponent who will roll the state forward with an action.</param>
	/// <returns>True if one action was executed.</returns>
	inline bool rollOppAction(GameState& gs, const ForwardModel& fm, const AgentParameters& agParams, int oppID)
	{
		//An opponent is to play in this state.
		std::shared_ptr<BaseActionScript> oppModel = agParams.getOpponentModel();
		if (oppModel)
		{
			//If an opponent model is defined, apply it.
			auto actions = fm.generateActions(gs, oppID);
			fm.advanceGameState(gs, oppModel->getAction(gs, actions, oppID));
			return true;
		}
		else
		{
			//No model defined, we skip opponent turn
			fm.advanceGameState(gs, Action::createEndAction(oppID));
		}

		return false;
	}


	/// <summary>
	/// Rolls the state forward for all OPPONENTS in the game. For all players with an ID != agParams.PLAYER_ID, the opponent model from 
	/// agParams is used to decide the action. If no opponent model is defined, nothing / skip turn is executed.
	/// </summary>
	/// <param name="gs">Game state to roll forward</param>
	/// <param name="fm">Forward model to roll the state forward and quering available actions.</param>
	/// <param name="agParams">Parameters of the agent who's calling this method.</param>
	/// <returns>The number of actions executed by this function.</returns>
	inline int rollOppOnly(GameState& gs, const ForwardModel& fm, const AgentParameters& agParams)
	{
		int actionsExecuted = 0;
		std::vector<int> canPlay = gs.whoCanPlay();
		for (int id : canPlay)
		{
			//Try to execute an action for this player if it's an opponent opponent
			if (id != agParams.PLAYER_ID && rollOppAction(gs, fm, agParams, id))  actionsExecuted++;
		}
		return actionsExecuted;
	}

	/// <summary>
	/// Rolls the state forward for all players in the game. For the player with an ID == agParams.PLAYER_ID, the action 'act' will be executed. 
	/// For the rest, the opponent model from agParams is used to decide the action. If no opponent model is defined, nothing / skip turn is executed.
	/// </summary>
	/// <param name="gs">Game state to roll forward</param>
	/// <param name="fm">Forward model to roll the state forward and quering available actions.</param>
	/// <param name="act">Action to execute for the player with id equals to agParams.PLAYER_ID</param>
	/// <param name="agParams">Parameters of the agent who's calling this method.</param>
	/// <returns>The number of actions executed by this function.</returns>
	inline int roll(GameState& gs, const ForwardModel& fm, const Action& act, int playerID, const AgentParameters& agParams)
	{
		int actionsExecuted = 0;
		std::vector<int> whoCanPlay = gs.whoCanPlay();

		for (int id : whoCanPlay)
		{
			if (id == playerID)
			{
				//This is the player for which we passed the action to execute.
				fm.advanceGameState(gs, act);
				actionsExecuted++;
			}
			//An opponent is to play in this state, execute:
			else if (agParams.PLAYER_ID != id)
			{
				rollOppAction(gs, fm, agParams, id);
				actionsExecuted++;
			}
		}
		return actionsExecuted;
	}
}

