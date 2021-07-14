#pragma once
#include <Stratega/Representation/Parameter.h>
#include <Stratega/ForwardModel/Action.h>
#include <unordered_map>
#include <vector>

namespace SGA
{
	struct Player
	{

	private:

		/// <summary>
		/// ID of this player, unique for players.
		/// </summary>
		int id;

		/// <summary>
		/// Indicates if this player can play at this time.
		/// </summary>
		bool canIPlay;

		/// <summary>
		/// Array of actions currently being executed by this player
		/// </summary>
		std::vector<Action> continuousActions;


		/// <summary>
		/// List of parameter values. Use GameState.getPlayerParameter(...) functions to retrieve this.
		/// </summary>
		std::vector<double> parameters;

		/// <summary>
		/// Actions that this player can execute in this game.
		/// </summary>
		std::vector<ActionInfo> attachedActions;

	public: 

		Player(int id, bool canPlay) :
			id(id), canIPlay(canPlay) {}
		 
		/// <summary>
		/// Indicates if a given action type can be executed by this player.
		/// </summary>
		[[nodiscard]] bool canExecuteAction(int actionTypeID) const;

		/// <summary>
		/// Returns the ActionInfo of an action type this player can execute.
		/// </summary>
		[[nodiscard]] const ActionInfo& getActionInfo(int actionTypeID) const;

		/// <summary>
		/// Returns the player ID defined for a neutral player.
		/// </summary>
		/// <returns>A constant ID for neutral player.</returns>
		static const int getNeutralPlayerID() { return -1; }

		/// <summary>
		/// Returns the ID of this player
		/// </summary>
		int getID() const { return id; }

		/// <summary>
		/// Indicates if this player can play at this time.
		/// </summary>
		bool canPlay() const { return canIPlay; }

		/// <summary>
		/// Sets if the player can play now or not.
		/// </summary>
		void setCanPlay(bool can) { canIPlay = can; }

		/// <summary>
		/// Returns the list of parameters, can't be modified.
		/// </summary>
		const std::vector<double>& getParameters() const { return parameters; }

		/// <summary>
		/// Returns the list of parameters of this player (can be modified)
		/// </summary>
		std::vector<double>& getParameters() { return parameters; }

		/// <summary>
		/// Returns a reference to a parameter value of this player.
		/// </summary>
		double& getParameter(int paramIdx) { return parameters[paramIdx]; }

		/// <summary>
		/// Returns a const value of a parameter of this player.
		/// </summary>
		const double& getParameter(int paramIdx) const { return parameters[paramIdx]; }

		/// <summary>
		/// Sets the parameter of this player to a certain value
		/// </summary>
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setParameter(int paramIdx, double val) { parameters[paramIdx] = val; }

		/// <summary>
		/// Sets a size for the vector of parameters of this player.
		/// </summary>
		void resizeParameters(int cap) { parameters.resize(cap); }

		/// <summary>
		/// Removes a continuous action from the vector of continuous actions of this player.
		/// </summary>
		/// <param name="idx">IDx of the action to remove.</param>
		void removeContinuousAction(int idx) { continuousActions.erase(continuousActions.begin() + idx); }

		/// <summary>
		/// Adds a continuous action to the list of this player.
		/// </summary>
		void addContinuousAction(Action newAction) { continuousActions.emplace_back(newAction); }

		/// <summary>
		/// Advances the tick counter of the continuous action with index 'idx'
		/// </summary>
		void advanceContinuousAction(int idx) { continuousActions[idx].elapsedTicks++; }

		/// <summary>
		/// Returns the list of continuous actions this player is executing.
		/// </summary>
		const std::vector<Action>& getContinuousActions() const { return continuousActions; }



		/// <summary>
		/// Returns the list of attached actions to this player.
		/// </summary>
		const std::vector<ActionInfo>& getAttachedActions() const { return attachedActions; }

		/// <summary>
		/// Returns the action info of the attached action with index 'idx'.
		/// </summary>
		const ActionInfo& getAttachedAction(int idx) const { return attachedActions[idx]; }

		/// <summary>
		/// Adds a new attached action to this player.
		/// </summary>
		void addAttachedAction(int actionTypeID, int lastExecutedTick) { attachedActions.emplace_back(ActionInfo{actionTypeID, lastExecutedTick}); }

		/// <summary>
		/// Sets the last tick on an attached action, indexed by 'idx'
		/// </summary>
		void setActionLastTick(int idx, int lastTick) { attachedActions[idx].lastExecutedTick = lastTick; }

		/// <summary>
		/// Reserves space for attached actions.
		/// </summary>
		void resAttachedActions(int cap) { attachedActions.reserve(cap); }

	};
}
