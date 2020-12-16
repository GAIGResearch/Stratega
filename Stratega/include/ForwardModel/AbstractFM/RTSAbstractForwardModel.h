#pragma once
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <Representation/AbstractGS/RTSGameState2.h>
#include <Representation/AbstractGS/Player.h>

#include "Representation/Path.h"

#include <chrono>

#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"

namespace  SGA
{
	class RTSAbstractForwardModel : public ForwardModel<RTSGameState2>
	{
	public:
		float deltaTime;
		
		RTSAbstractForwardModel()
			: deltaTime(1. / 60.)
			/*winCondition(),
			unitTypeID(),
			actionSpace(generateDefaultActionSpace())*/
		{
		}

		void advanceGameState(RTSGameState2& state, const Action& action) const override;

		std::vector<Action> generateActions(RTSGameState2& state) const override;
		std::vector<Action> generateActions(RTSGameState2& state, int playerID) const override;

		/*void executeMove(RTSFMState& state, RTSUnit& unit) const;
		void executeAttack(RTSFMState& state, RTSUnit& unit) const;
		void executeHeal(RTSFMState& state, RTSUnit& unit) const;*/

		void resolveUnitCollisions(RTSGameState2& state) const;
		void resolveEnvironmentCollisions(RTSGameState2& state) const;

		bool buildNavMesh(RTSGameState2& state, NavigationConfig config) const;
		Path findPath(RTSGameState2& state, Vector2f startPos, Vector2f endPos) const;

		bool checkGameIsFinished(RTSGameState2& state) const;
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
