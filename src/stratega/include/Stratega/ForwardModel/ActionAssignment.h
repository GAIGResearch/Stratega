#pragma once

#include <Stratega/ForwardModel/Action.h>
#include <unordered_map>

namespace SGA
{
	/// <summary>
	/// A collection of action-assignments to entities and players in a game.
	/// </summary>
	class ActionAssignment
	{

	private: 

		std::unordered_map<int, Action> entityActions;
		std::unordered_map<int, Action> playerActions;

	public:

		/// <summary>
		/// Assigns the action to the corresponding entity or player. Any existing assignment will be overwritten.
		/// </summary>
		/// <param name="newAction">The action to be assigned. The first target of the action is used to determine who executes this action.</param>
		void assignActionOrReplace(Action newAction);

		/// <summary>
		/// Copies the given action-assignment into this object. Any existing assignment will be overwritten in case of a conflict.
		/// </summary>
		/// <param name="action">The assignment that should be merged into this one.</param>
		void merge(const ActionAssignment& action);

		/// <summary>
		/// Deletes all assignments.
		/// </summary>
		void clear();

		/// <summary>
		/// Returns the assignment for an entity.
		/// </summary>
		/// <param name="entityID">The ID of the entity.</param>
		/// <returns>The assigned action or nullptr if no action was assigned.</returns>
		Action* getEntityAction(int entityID);

		/// <summary>
		/// Returns the assignment for an player.
		/// </summary>
		/// <param name="playerID">The ID of the player.</param>
		/// <returns>The assigned action or nullptr if no action was assigned.</returns>
		Action* getPlayerAction(int playerID);

		/// <summary>
		/// Returns the amount of actions that are assigned to entities and players.
		/// </summary>
		size_t getAssignmentCount() const;
		
		const std::unordered_map<int, Action>& getEntityActions() const;
		const std::unordered_map<int, Action>& getPlayerActions() const;

		/// <summary>
		/// Constructs a ActionAssignment from a single action, useful for TBS-Games.
		/// </summary>
		/// <param name="a">The action that should be contained in the ActionAssignment.</param>
		/// <returns>An ActionAssignment containing a single assignment.</returns>
		static ActionAssignment fromSingleAction(Action a);

		/// <summary>
		/// Generates an ActionAssignment used by the game to end the tick/turn.
		/// </summary>
		/// <param name="playerID">ID of the player for which the End action is created</param>
		/// <returns>ActionAssignment that ends the current turn for the plater ID supplied</returns>
		static ActionAssignment createEndActionAssignment(int playerID);

	};
}