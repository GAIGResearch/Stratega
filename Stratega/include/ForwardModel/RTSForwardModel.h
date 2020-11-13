#pragma once
#include <vector>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/RTSAction.h>
#include <ForwardModel/FMState.h>
#include <Representation/RTSGameState.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"


namespace SGA
{
	
	class RTSForwardModel : public ForwardModelBase<RTSGameState, RTSAction>
	{
	public:
		double deltaTime = 1. / 60.;

		WinConditionType winCondition;
		int unitTypeID;

		void advanceGameState(RTSGameState& state, const RTSAction& action) const override;
		
		std::vector<RTSAction> getActions(RTSGameState& state) const override;
		std::vector<RTSAction> getActions(RTSGameState& state, int playerID) const override;

		void generateMoves(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		void generateAttacks(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		void generateHeals(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		RTSAction generateEndTickAction() const;

		bool validateMove(RTSGameState& state, const RTSAction& action) const;
		bool validateAttack(RTSGameState& state, const RTSAction& action) const;
		bool validateHeal(RTSGameState& state, const RTSAction& action) const;
		
		void executeMove(RTSFMState& state, RTSUnit& unit) const;
		void executeAttack(RTSFMState& state, RTSUnit& unit) const;
		void executeHeal(RTSFMState& state, RTSUnit& unit) const;

		void resolveUnitCollisions(RTSFMState& state) const;
		void resolveEnvironmentCollisions(RTSFMState& state) const;
		
		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
		bool canPlayerPlay(RTSPlayer& player) const;
	};

}
