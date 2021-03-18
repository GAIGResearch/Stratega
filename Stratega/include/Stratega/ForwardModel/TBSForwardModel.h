#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <chrono>
#include <Stratega/Representation/Path.h>
#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"
namespace  SGA
{
	struct GameState;

	/// <summary>
/// A collection of action-assignments to entities and players in a game.
/// Executing this action with <see cref="SGA::RTSForwardModel::advanceGameState(GameState&, const RTSAction&)"/> will advance the game by one tick.
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
	
	class ForwardModel : public EntityForwardModel
	{
	public:
		ForwardModel()
			: deltaTime(1. / 60.)
		{
		}
		
		void advanceGameState(GameState& state, const Action& action) const;

		void endTurn(GameState& state) const;

		virtual std::vector<Action> generateActions(GameState& state) const;

		virtual std::vector<Action> generateActions(GameState& state, int playerID) const;

		virtual bool isValid(const GameState& state, const Action& action) const;

		bool checkGameIsFinished(GameState& state) const;

		//RTS stuff
		float deltaTime;
		
		void advanceGameState(GameState& state, const RTSAction& action) const;		

		void moveEntities(GameState& state) const;
		void resolveEntityCollisions(GameState& state) const;
		void resolveEnvironmentCollisions(GameState& state) const;

		bool buildNavMesh(GameState& state, NavigationConfig config) const;
		Path findPath(const GameState& state, Vector2f startPos, Vector2f endPos) const;
	};
}
