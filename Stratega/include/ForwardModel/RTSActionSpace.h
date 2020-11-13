#pragma once
#include <Representation/RTSGameState.h>
#include <ForwardModel/RTSAction.h>
#include <ForwardModel/IActionSpace.h>

namespace SGA
{
	class RTSActionSpace : public IActionSpace<RTSGameState, RTSAction>
	{
	public:
		int targetPlayerID;

		std::vector<RTSAction> generateActions(RTSGameState& gameState) override;
		std::vector<RTSAction> generateActions(RTSGameState& gameState, int playerID);
		void generateMoves(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		void generateAttacks(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		void generateHeals(RTSUnit& unit, std::vector<RTSAction>& actionBucket) const;
		RTSAction generateEndTickAction() const;
	};
}
