#pragma once
#include <vector>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/TBSActionSpace.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/FMState.h>
#include <Representation/RTSGameState.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"


namespace SGA
{
	
	class RTSForwardModel : public ForwardModelBase<RTSGameState, Action<Vector2f>>
	{
	public:
		double deltaTime = 1. / 60.;
		
		void advanceGameState(RTSGameState& state, const Action<Vector2f>& action) const override;
		void advanceGameState(RTSGameState& state, const std::vector<Action<Vector2f>>& action) const override;

		std::unique_ptr<ActionSpace<Vector2f>> getActions(RTSGameState& state) const
		{
			return std::unique_ptr<ActionSpace<Vector2f>>(generateActions(state));
		}
		std::unique_ptr<ActionSpace<Vector2f>> getActions(RTSGameState& state, int playerID) const
		{
			return std::unique_ptr<ActionSpace<Vector2f>>(generateActions(state, playerID));
		}

		void generateMoves(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const;
		void generateAttacks(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const;
		void generateHeals(RTSUnit& unit, ActionSpace<Vector2f>& actionBucket) const;

		bool validateMove(RTSGameState& state, const Action<Vector2f>& action) const;
		bool validateAttack(RTSGameState& state, const Action<Vector2f>& action) const;
		bool validateHeal(RTSGameState& state, const Action<Vector2f>& action) const;
		
		void executeMove(RTSFMState& state, RTSUnit& unit) const;
		void executeAttack(RTSFMState& state, RTSUnit& unit) const;
		void executeHeal(RTSFMState& state, RTSUnit& unit) const;

		void resolveUnitCollisions(RTSFMState& state) const;
		void resolveEnvironmentCollisions(RTSFMState& state) const;
		
		bool buildNavMesh(RTSGameState& state) const;
		Path findPath(RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

	private:
		ActionSpace<Vector2f>* generateActions(RTSGameState& state) const override;
		ActionSpace<Vector2f>* generateActions(RTSGameState& state, int playerID) const override;
	};

}
