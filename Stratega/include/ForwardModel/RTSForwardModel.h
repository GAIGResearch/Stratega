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

		/*void executeMove(RTSFMState& state, RTSUnit& unit) const;
		void executeAttack(RTSFMState& state, RTSUnit& unit) const;
		void executeHeal(RTSFMState& state, RTSUnit& unit) const;*/

		void resolveUnitCollisions(RTSGameState& state) const;
		void resolveEnvironmentCollisions(RTSGameState& state) const;

		bool buildNavMesh(RTSGameState& state, NavigationConfig config) const;
		Path findPath(RTSGameState& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState& state) const;
		bool canPlayerPlay(Player& player) const;

		//// ActionSpaces
		//void setActionSpace(std::unique_ptr<RTSActionSpace> as)
		//{
		//	actionSpace = std::move(as);
		//}

		//RTSActionSpace& getActionSpace() const
		//{
		//	return *actionSpace;
		//}

		/*std::unique_ptr<RTSActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<RTSActionSpace>();
		}*/

	/*private:
		std::shared_ptr<RTSActionSpace> actionSpace;*/
	};
}
