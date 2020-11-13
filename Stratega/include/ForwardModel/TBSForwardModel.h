#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAction.h>
#include <ForwardModel/Effect.h>
#include <ForwardModel/FMState.h>
#include <Configuration/WinConditionType.h>
#include <ForwardModel/ForwardModelBase.h>

namespace SGA
{
	class TBSForwardModel : public ForwardModelBase<TBSGameState, TBSAction>
	{
	public:
		WinConditionType winCondition;
		int unitTypeID;
		
		void advanceGameState(TBSGameState& state, const TBSAction& action) const override;
		void advanceGameState(TBSGameState& state, const TBSAction& action, std::vector<TBSAction>& actionSpace) const;

		std::vector<TBSAction> getActions(TBSGameState& state) const override;
		std::vector<TBSAction> getActions(TBSGameState& state, int playerID) const override;
		
		bool isValid(TBSGameState& state, const TBSAction& action) const;

		void generateMoveActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generateAttackActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generatePushActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generateHealActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const;
		void generateEndOfTurnActions(TBSGameState& state, int playerID, std::vector<SGA::TBSAction>& actionBucket) const;
		
		bool executeMove(FMState& state, const TBSAction& action) const;
		bool executeAttack(FMState& state, const TBSAction& action) const;
		bool executePush(FMState& state, const TBSAction& action) const;
		bool executeHeal(FMState& state, const TBSAction& action) const;
		bool executeEndOfTurn(FMState& state, const TBSAction& action) const;

		bool validateMove(TBSGameState& state, const TBSAction& action) const;
		bool validateAttack(TBSGameState& state, const TBSAction& action) const;
		bool validatePush(TBSGameState& state, const TBSAction& action) const;
		bool validateHeal(TBSGameState& state, const TBSAction& action) const;
		bool validateEndOfTurn(TBSGameState& state, const TBSAction& action) const;

		// Utility methods for game logic
		bool isWalkable(TBSGameState& state, const Vector2i& position) const;
		void moveUnit(FMState& state, TBSUnit& u, Vector2i newPosition) const;
		void killUnit(FMState& state, TBSUnit& u) const;
		void damageUnit(FMState& state, TBSUnit& u, int damageAmount) const;
		
		void endTurn(FMState& state) const;
		void initTurn(FMState& state) const;
		bool checkGameIsFinished(TBSGameState& state) const;
		bool canPlayerPlay(TBSPlayer& player) const;
		
		// Effect handling
		void addOnTileEnterEffect(Effect&& effect);
		void addUnitEndOfTurnEffect(Effect&& effect);
		void executeEndOfTurnTrigger(FMState& state) const;
		void executeOnEnterTileTrigger(FMState& state, TBSUnit& targetUnit) const;
		bool isConditionFulfilled(const Effect& effect, TBSUnit& targetUnit) const;
		void executeEffect(FMState& state, const Effect& effect, TBSUnit& targetUnit) const;

		
	protected:
		std::vector<Effect> unitEndOfTurnEffects;
		std::vector<Effect> onTileEnterEffects;
		
	};
}
