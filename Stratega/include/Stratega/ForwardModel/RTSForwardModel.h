#pragma once
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Representation/Path.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"

namespace  SGA
{
	/// <summary>
	/// Is the default <see cref="SGA::yForwardModel"/> for RTS games, it contains specific methods.
	/// Each <see cref="SGA::RTSForwardModel::advanceGameState()"/> call move all the entities and resolve the collisions before
	/// executing all the received actions.
	/// After executing the actions it updates the entities that should be
	/// removed before checking if the game is over.
	/// The <see cref="SGA::RTSForwardModel::advanceGameState()"/> accepts a list of actions or one single action.
	/// </summary>
	class RTSForwardModel : public ForwardModel
	{

	private:

		double deltaTime;

	public:
		
		
		RTSForwardModel()
			: deltaTime(1. / 60.)
		{
		}
		
		/// <summary>
		/// Moves all the entities and resolves collisions before and after executing an action in a given <see cref="SGA::GameState"/>
		/// After executing the actions it updates the entities that should be
		/// removed before checking if the game is over.
		/// </summary>
		void advanceGameState(GameState& state, const Action& action) const override;
		
		/// <summary>
		/// Moves all the entities and resolves collisions before and after executing a list of actions in a given <see cref="SGA::GameState"/>
		/// After executing the actions it updates the entities that should be
		/// removed before checking if the game is over.
		/// </summary>
		void advanceGameState(GameState& state, const ActionAssignment& action) const override;

		std::unique_ptr<ForwardModel> clone() const override;

		GameType getGameType() const override { return SGA::GameType::RTS; }

		/// <summary>
		/// Moves all the entities that have a current path and they did not reach their destination.
		/// If the entity has a path it moves the entity through all the path points one after the
		/// other until reaching the last one.
		/// </summary>
		void moveEntities(GameState& state) const;
		/// <summary>
		/// Resolves collisions between entities in a basic way computing the penetration depth and pushing them way in the opposite direction.
		/// </summary>
		void resolveEntityCollisions(GameState& state) const;
		/// <summary>
		/// Resolves collisions between entities and the tiles that are not walkable in a basic way
		/// computing the penetration depth and pushing them way in the opposite direction.
		/// </summary>
		void resolveEnvironmentCollisions(GameState& state) const;


		/// <summary>
		/// Generate a complete new Navmesh using Recast framework.
		/// Is a heavy computation task depending the size of the tilemap.
		/// </summary>
		bool buildNavMesh(GameState& state, NavigationConfig config) const;
		/// <summary>
		/// Returns a <see cref="SGA::Path"/> inside the Navmesh between the start and end positons.
		/// </summary>
		Path findPath(const GameState& state, Vector2f startPos, Vector2f endPos) const;

		/// <summary>
		/// Checks if the game is finished by current limit or because a player has won.
		/// </summary>
		bool checkGameIsFinished(GameState& state) const;
	};
}
