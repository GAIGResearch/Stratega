#pragma once
#include <ForwardModel/ForwardModel.h>
#include <Representation/RTSGameState.h>
#include <Representation/Player.h>

#include "Representation/Path.h"

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"

namespace  SGA
{
	class RTSForwardModel : public ForwardModel<RTSGameState>
	{
	public:
		float deltaTime;
		
		RTSForwardModel()
			: deltaTime(1. / 60.)
			/*winCondition(),
			unitTypeID(),
			actionSpace(generateDefaultActionSpace())*/
		{
		}

		void advanceGameState(RTSGameState& state, const Action& action) const override;

		std::vector<Action> generateActions(RTSGameState& state) const override;
		std::vector<Action> generateActions(RTSGameState& state, int playerID) const override;

		void resolveUnitCollisions(RTSGameState& state) const;
		void resolveEnvironmentCollisions(RTSGameState& state) const;

		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
		bool canPlayerPlay(Player& player) const;
	};
}
