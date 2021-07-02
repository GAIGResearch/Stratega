#pragma once
#include <Stratega/Representation/Parameter.h>
#include <Stratega/ForwardModel/Action.h>
#include <unordered_map>
#include <vector>

namespace SGA
{
	struct Player
	{
		/// <summary>
		/// Reserved ID for a neutral player.
		/// </summary>
		static const int NEUTRAL_PLAYER_ID = -1;
		
		/// <summary>
		/// ID of this player, unique for players.
		/// </summary>
		int id;

		/// <summary>
		/// Indicates if this player can play at this time.
		/// </summary>
		bool canPlay;

		/// <summary>
		/// Array of actions currently being executed by this player
		/// </summary>
		std::vector<Action> continuousAction;

		/// <summary>
		/// List of parameter values. Use GameState.getPlayerParameter(...) functions to retrieve this.
		/// </summary>
		std::vector<double> parameters;

		/// <summary>
		/// Actions that this player can execute in this game.
		/// </summary>
		std::vector<ActionInfo> attachedActions;

		/// <summary>
		/// Indicates if a given action type can be executed by this player.
		/// </summary>
		[[nodiscard]] bool canExecuteAction(int actionTypeID) const;

		/// <summary>
		/// Returns the ActionInfo of an action type this player can execute.
		/// </summary>
		[[nodiscard]] const ActionInfo& getActionInfo(int actionTypeID) const;
	};
}
