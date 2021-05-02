#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/Representation/Path.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"

namespace  SGA
{
	class RTSForwardModel : public EntityForwardModel
	{
	public:
		double deltaTime;
		
		RTSForwardModel()
			: deltaTime(1. / 60.)
		{
		}

		void advanceGameState(GameState& state, const Action& action) const override;
		void advanceGameState(GameState& state, const ActionAssignment& action) const override;
		std::unique_ptr<EntityForwardModel> clone() const override;
		
		void moveEntities(GameState& state) const;
		void resolveEntityCollisions(GameState& state) const;
		void resolveEnvironmentCollisions(GameState& state) const;

		bool buildNavMesh(GameState& state, NavigationConfig config) const;
		Path findPath(const GameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(GameState& state) const;
	};
}
