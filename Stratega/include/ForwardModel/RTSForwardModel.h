#pragma once
#include <vector>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/RTSAction.h>
#include <ForwardModel/FMState.h>
#include <Representation/RTSGameState.h>
#include <ForwardModel/RTSActionSpace.h>

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"


namespace SGA
{
	
	class RTSForwardModel : public ForwardModelBase<RTSGameState, RTSAction>
	{
	public:
		double deltaTime;

		WinConditionType winCondition;
		int unitTypeID;


		RTSForwardModel()
			: deltaTime(1. / 60.),
			  winCondition(),
			  unitTypeID(),
			  actionSpace(generateDefaultActionSpace())
		{
		}

		void advanceGameState(RTSGameState& state, const RTSAction& action) const override;
		
		std::vector<RTSAction> generateActions(RTSGameState& state) const override;
		std::vector<RTSAction> generateActions(RTSGameState& state, int playerID) const override;
		
		void executeMove(RTSFMState& state, RTSUnit& unit) const;
		void executeAttack(RTSFMState& state, RTSUnit& unit) const;
		void executeHeal(RTSFMState& state, RTSUnit& unit) const;

		void resolveUnitCollisions(RTSFMState& state) const;
		void resolveEnvironmentCollisions(RTSFMState& state) const;
		
		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
		bool canPlayerPlay(RTSPlayer& player) const;

		// ActionSpaces
		void setActionSpace(std::unique_ptr<RTSActionSpace> as)
		{
			actionSpace = std::move(as);
		}

		RTSActionSpace& getActionSpace() const
		{
			return *actionSpace;
		}

		std::unique_ptr<RTSActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<RTSActionSpace>();
		}

	private:
		std::shared_ptr<RTSActionSpace> actionSpace;
	};

}
