#pragma once
#include <Stratega/Representation/Parameter.h>
#include <Stratega/ForwardModel/Action.h>
#include <unordered_map>
#include <vector>
#include <Stratega/Representation/Buff.h>
namespace SGA
{
	struct GameState;

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
		std::unordered_map<int, double> parameters;

		/// <summary>
		/// Values for the max parameters value of this entity. Indexed by ID. Use getMaxParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> maxParameters;

		/// <summary>
		/// Values for the min parameters value of this entity. Indexed by ID. Use getMinParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> minParameters;

		/// <summary>
		/// Actions that this player can execute in this game.
		/// </summary>
		std::vector<ActionInfo> attachedActions;

		/// <summary>
		/// Array of buffs currently applied to this entity
		/// </summary>
		std::vector<Buff> buffs;


	public: 

		Player(int newID, bool newCanPlay) :
			id(newID), canIPlay(newCanPlay) {}
		 
		/// <summary>
		/// Indicates if a given action type can be executed by this player.
		/// </summary>
		bool canExecuteAction(int actionTypeID) const;

		/// <summary>
		/// Returns the ActionInfo of an action type this player can execute.
		/// </summary>
		const ActionInfo& getActionInfo(int actionTypeID) const;

		/// <summary>
		/// Returns the player ID defined for a neutral player.
		/// </summary>
		/// <returns>A constant ID for neutral player.</returns>
		static int getNeutralPlayerID() { return -1; }

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
		const std::unordered_map<int, double>& getParameters() const { return parameters; }

		/// <summary>
		/// Returns the list of parameters of this player (can be modified)
		/// </summary>
		std::unordered_map<int, double>& getParameters() { return parameters; }

		/// <summary>
		/// Returns a reference to a parameter value of this player.
		/// </summary>
		double& getRawParameterAt(int paramIdx) { return parameters.find(paramIdx)->second; }

		/// <summary>yer to a certain value
		/// </summary>
		/// Returns a const value of a parameter of this player.
		/// </summary>
		const double& getRawParameterAt(int paramIdx) const { return parameters.find(paramIdx)->second; }

		/// <summary>
		/// Gets a specific parameters value, by index 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double getParameterAt(int paramIdx) { return parameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Gets a specific max parameters value, by index 
		/// <summary>
		/// <returns>The max parameter value.</returns>
		double getMaxParameterAt(int paramIdx) { return maxParameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Gets a specific min parameters value, by index 
		/// <summary>
		/// <returns>The min parameter value.</returns>
		double getMinParameterAt(int paramIdx) { return minParameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setParameter(int paramIdx, double val) { parameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setMaxParameter(int paramIdx, double val) { maxParameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setMinParameter(int paramIdx, double val) { minParameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets a size for the vector of parameters of this player.
		/// </summary>
		void resizeParameters(int cap)
		{
			/*parameters.resize(static_cast<size_t>(cap));*//*
			maxParameters.resize(static_cast<size_t>(cap));
			minParameters.resize(static_cast<size_t>(cap));*/
		}
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
		void advanceContinuousAction(int idx) { continuousActions[static_cast<size_t>(idx)].incTicks(); }

		/// <summary>
		/// Returns the list of continuous actions this player is executing.
		/// </summary>
		const std::vector<Action>& getContinuousActions() const { return continuousActions; }


		/// <summary>
		/// Gets the list of buffs attached to this entity. Modifiable.
		/// <summary>
		/// <returns>The list of buffs attached to this entity.</returns>
		std::vector<Buff>& getBuffs() { return buffs; }

		/// <summary>
		/// Gets the list of buffs attached to this entity. 
		/// <summary>
		/// <returns>The list of buffs attached to this entity.</returns>
		const std::vector<Buff>& getBuffs() const { return buffs; }

		/// <summary>
		/// Add buff to the player
		/// <summary>
		void addBuff(Buff b)
		{
			buffs.emplace_back(b);
		}

		/// <summary>
		/// Recompute all the parameteres with the applied buffs
		/// <summary>
		void recomputeStats(GameState& state);

		/// <summary>
		/// Remove all the buffs of the same type
		/// <summary>
		void removeBuffsOfType(const BuffType& type)
		{
			auto it = buffs.begin();
			while (it != buffs.end())
			{
				if (it->getType().getID() == type.getID())
				{
					it = buffs.erase(it);
				}
				else it++;
			}
		}

		/// <summary>
		/// Recompute all the buffs
		/// <summary>
		void removeAllBuffs()
		{
			buffs.clear();
		}

		/// <summary>
		/// Check if player has a buff type applied
		/// <summary>
		bool hasBuff(int typeID) const
		{
			for (auto& buff : buffs)
			{
				if (buff.getType().getID() == typeID)
					return true;
			}

			return false;
		}

		/// <summary>
		/// Returns the list of attached actions to this player.
		/// </summary>
		const std::vector<ActionInfo>& getAttachedActions() const { return attachedActions; }

		/// <summary>
		/// Returns the action info of the attached action with index 'idx'.
		/// </summary>
		const ActionInfo& getAttachedAction(int idx) const { return attachedActions[static_cast<size_t>(idx)]; }

		/// <summary>
		/// Adds a new attached action to this player.
		/// </summary>
		void addAttachedAction(int actionTypeID, int lastExecutedTick) { attachedActions.emplace_back(ActionInfo{actionTypeID, lastExecutedTick}); }

		/// <summary>
		/// Sets the last tick on an attached action, indexed by 'idx'
		/// </summary>
		void setActionLastTick(int idx, int lastTick) { attachedActions[static_cast<size_t>(idx)].lastExecutedTick = lastTick; }

		/// <summary>
		/// Reserves space for attached actions.
		/// </summary>
		void resAttachedActions(int cap) { attachedActions.reserve(static_cast<size_t>(cap)); }

	};
}
