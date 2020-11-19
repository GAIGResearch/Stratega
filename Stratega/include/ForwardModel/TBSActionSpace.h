#pragma once

#include<ForwardModel/IActionSpace.h>
#include<ForwardModel/TBSAction.h>

namespace SGA
{
	class TBSActionSpace : public IActionSpace<TBSGameState, TBSAction>
	{
	public:
		std::vector<TBSAction> generateActions(TBSGameState& gameState) override;
		std::vector<TBSAction> generateActions(TBSGameState& gameState, int playerID);

		void generateMoveActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generateAttackActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generatePushActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generateHealActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		TBSAction TBSActionSpace::generateEndOfTurnAction(TBSGameState& state, int playerID) const;
	};
}
