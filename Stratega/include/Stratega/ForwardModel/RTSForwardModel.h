#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/Representation/RTSGameState.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/Path.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"

namespace  SGA
{
	/// <summary>
	/// A collection of action-assignments to entities and players in a game.
	/// Executing this action with <see cref="SGA::RTSForwardModel::advanceGameState(RTSGameState&, const RTSAction&)"/> will advance the game by one tick.
	/// </summary>
	class RTSAction
	{
		std::unordered_map<int, Action> entityActions;
		std::unordered_map<int, Action> playerActions;

	public:
		/// <summary>
		/// Assigns the action to the corresponding entity or player. Any existing assignment will be overwritten.
		/// </summary>
		/// <param name="newAction">The action to be assigned. The first target of the action is used to determine who executes this action.</param>
		void assignActionOrReplace(Action newAction);

		/// <summary>
		/// Merges with the given action by copying all assignments. Any existing assignment will be overwritten in case of a conflict.
		/// </summary>
		/// <param name="action">The action that should be merged into this one.</param>
		void merge(const RTSAction& action);

		/// <summary>
		/// Deletes all assignments.
		/// </summary>
		void clear();
		
		/// <summary>
		/// Returns the assignment for an entity.
		/// </summary>
		/// <param name="entityID">The ID of the entity.</param>
		/// <returns>The assigned action or nullptr if no action was assigned.</returns>
		[[nodiscard]] Action* getEntityAction(int entityID);
		
		/// <summary>
		/// Returns the assignment for an player.
		/// </summary>
		/// <param name="playerID">The ID of the player.</param>
		/// <returns>The assigned action or nullptr if no action was assigned.</returns>
		[[nodiscard]] Action* getPlayerAction(int playerID);

		[[nodiscard]] const std::unordered_map<int, Action>& getEntityActions() const;
		[[nodiscard]] const std::unordered_map<int, Action>& getPlayerActions() const;
	};
	
	class RTSForwardModel : public EntityForwardModel
	{
	public:
		float deltaTime;
		
		RTSForwardModel()
			: deltaTime(1. / 60.)
		{
		}

		void advanceGameState(RTSGameState& state, const RTSAction& action) const;

		std::vector<Action> generateActions(RTSGameState& state) const;
		std::vector<Action> generateActions(RTSGameState& state, int playerID) const;

		void moveEntities(RTSGameState& state) const;
		void resolveEntityCollisions(RTSGameState& state) const;
		void resolveEnvironmentCollisions(RTSGameState& state) const;

		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(const RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
	};
}
