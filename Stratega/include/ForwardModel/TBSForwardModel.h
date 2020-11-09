#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/Effect.h>
#include <ForwardModel/FMState.h>
#include <Configuration/WinConditionType.h>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/TBSActionSpace.h>

namespace SGA
{
	class TBSForwardModel : public ForwardModelBase<TBSGameState, Action<Vector2i>>
	{
	public:
		WinConditionType winCondition;
		int unitTypeID;
		
		void advanceGameState(TBSGameState& state, const Action<Vector2i>& action) const override;
		void advanceGameState(TBSGameState& state, const Action<Vector2i>& action, ActionSpace<Vector2i>& actionSpace) const;

		std::unique_ptr<ActionSpace<Vector2i>> getActions(TBSGameState& state) const
		{
			return std::unique_ptr<ActionSpace<Vector2i>>(generateActions(state));
		}

		std::unique_ptr<ActionSpace<Vector2i>> getActions(TBSGameState& state, int playerID) const
		{
			return std::unique_ptr<ActionSpace<Vector2i>>(generateActions(state, playerID));
		}
		
		bool isValid(TBSGameState& state, const Action<Vector2i>& action) const;

		void generateMoveActions(TBSUnit& unit, ActionSpace<Vector2i>& actionBucket) const;
		void generateAttackActions(TBSUnit& unit, ActionSpace<Vector2i>& actionBucket) const;
		void generatePushActions(TBSUnit& unit, ActionSpace<Vector2i>& actionBucket) const;
		void generateHealActions(TBSUnit& unit, ActionSpace<Vector2i>& actionBucket) const;
		void generateEndOfTurnActions(TBSGameState& state, int playerID, ActionSpace<Vector2i>& actionBucket) const;
		
		bool executeMove(FMState& state, const Action<Vector2i>& action) const;
		bool executeAttack(FMState& state, const Action<Vector2i>& action) const;
		bool executePush(FMState& state, const Action<Vector2i>& action) const;
		bool executeHeal(FMState& state, const Action<Vector2i>& action) const;
		bool executeEndOfTurn(FMState& state, const Action<Vector2i>& action) const;

		bool validateMove(TBSGameState& state, const Action<Vector2i>& action) const;
		bool validateAttack(TBSGameState& state, const Action<Vector2i>& action) const;
		bool validatePush(TBSGameState& state, const Action<Vector2i>& action) const;
		bool validateHeal(TBSGameState& state, const Action<Vector2i>& action) const;
		bool validateEndOfTurn(TBSGameState& state, const Action<Vector2i>& action) const;

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

		ActionSpace<Vector2i>* generateActions(TBSGameState& state) const override;
		ActionSpace<Vector2i>* generateActions(TBSGameState& state, int playerID) const override;
		
	};
}
