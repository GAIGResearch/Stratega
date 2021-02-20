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
	class RTSForwardModel : public EntityForwardModel
	{
	public:
		float deltaTime;
		
		RTSForwardModel()
			: deltaTime(1. / 60.)
		{
		}

		void advanceGameState(RTSGameState& state, const Action& action) const;

		std::vector<Action> generateActions(RTSGameState& state) const;
		std::vector<Action> generateActions(RTSGameState& state, int playerID) const;

		void resolveUnitCollisions(RTSGameState& state) const;
		void resolveEnvironmentCollisions(RTSGameState& state) const;

		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(const RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
	};
}
