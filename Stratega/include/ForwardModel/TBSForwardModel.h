#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAction.h>
#include <ForwardModel/Effect.h>
#include <ForwardModel/FMState.h>
#include <Configuration/WinConditionType.h>
#include <ForwardModel/ForwardModelBase.h>
#include <ForwardModel/TBSActionSpace.h>

namespace SGA
{
	class TBSForwardModel : public ForwardModelBase<TBSGameState, TBSAction>
	{
	public:
		TBSForwardModel()
			: winCondition(),
			  unitTypeID(),
			  unitEndOfTurnEffects(),
			  onTileEnterEffects(),
			  actionSpace(generateDefaultActionSpace())
		{
		}

		WinConditionType winCondition;
		int unitTypeID;
		
		std::vector<Effect> unitEndOfTurnEffects;
		std::vector<Effect> onTileEnterEffects;
		std::shared_ptr<TBSActionSpace> actionSpace;

		void advanceGameState(TBSGameState& state, const TBSAction& action) const override;
		void advanceGameState(TBSGameState& state, const TBSAction& action, std::vector<TBSAction>& actionSpace) const;

		std::vector<TBSAction> generateActions(TBSGameState& state) const override;
		std::vector<TBSAction> generateActions(TBSGameState& state, int playerID) const override;
		
		bool isValid(TBSGameState& state, const TBSAction& action) const;

		void executeMove(FMState& state, const TBSAction& action) const;
		void executeAttack(FMState& state, const TBSAction& action) const;
		void executePush(FMState& state, const TBSAction& action) const;
		void executeHeal(FMState& state, const TBSAction& action) const;
		void executeEndOfTurn(FMState& state, const TBSAction& action) const;

		// Utility methods for game logic
		bool canKill(const TBSGameState& state, Vector2i pos) const;
		
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

		// ActionSpaces
		void setActionSpace(std::unique_ptr<TBSActionSpace> as)
		{
			actionSpace = std::move(as);
		}

		TBSActionSpace& getActionSpace() const
		{
			return *actionSpace;
		}

		std::unique_ptr<TBSActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<TBSActionSpace>();
		}
		
	};
}
