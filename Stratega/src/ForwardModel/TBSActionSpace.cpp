#include <ForwardModel/TBSActionSpace.h>

namespace SGA
{
	std::vector<TBSAction> TBSActionSpace::generateActions(TBSGameState& gameState)
	{
		return generateActions(gameState, gameState.currentPlayer);
	}

	std::vector<TBSAction> TBSActionSpace::generateActions(TBSGameState& gameState, int playerID)
	{
		std::vector<TBSAction> actionBucket;
		for (auto* unit : gameState.getPlayer(playerID)->getUnits())
		{
			if (unit->numActionsExecuted >= unit->getType().actionsPerTurn)
			{
				continue;
			}

			for (auto actionType : unit->getType().tbsActions)
			{
				if (!unit->getType().canRepeatActions && unit->executedActionTypes.find(actionType) != unit->executedActionTypes.end())
				{
					continue;
				}

				switch (actionType)
				{
				case TBSActionType::Attack: generateAttackActions(*unit, actionBucket); break;
				case TBSActionType::Move: generateMoveActions(*unit, actionBucket); break;
				case TBSActionType::Heal: generateHealActions(*unit, actionBucket); break;
				case TBSActionType::Push: generatePushActions(*unit, actionBucket); break;
				default: throw std::runtime_error("Unit can execute an invalid action-type");
				}
			}
		}

		generateEndOfTurnActions(gameState, playerID, actionBucket);
		return actionBucket;
	}
	
	void TBSActionSpace::generateMoveActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		auto moveRange = unit.getRange();
		auto startCheckPositionX = std::max(0, unit.getPosition().x - moveRange);
		auto endCheckPositionX = std::min(state.getBoard().getWidth() - 1, unit.getPosition().x + moveRange);
		auto startCheckPositionY = std::max(0, unit.getPosition().y - moveRange);
		auto endCheckPositionY = std::min(state.getBoard().getHeight() - 1, unit.getPosition().y + moveRange);
		for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
		{
			for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
			{
				TBSAction action{ TBSActionType::Move, unit.getPlayerID(), unit.getUnitID(), SGA::Vector2i(x, y), -1 };
				if (action.validate(state))
				{
					actionBucket.emplace_back(action);
				}
			}
		}
	}

	void TBSActionSpace::generateAttackActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (const auto& targetUnit : state.getUnits())
		{
			TBSAction attackAction{ TBSActionType::Attack, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if (attackAction.validate(state))
			{
				actionBucket.emplace_back(attackAction);
			}
		}
	}

	void TBSActionSpace::generatePushActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (const auto& targetUnit : state.getUnits())
		{
			TBSAction pushAction{ TBSActionType::Push, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if (pushAction.validate(state))
			{
				actionBucket.emplace_back(pushAction);
			}
		}
	}

	void TBSActionSpace::generateHealActions(TBSUnit& unit, std::vector<TBSAction>& actionBucket) const
	{
		auto& state = unit.state.get();
		for (const auto& targetUnit : state.getUnits())
		{
			TBSAction healAction{ TBSActionType::Heal, unit.getPlayerID(), unit.getUnitID(), targetUnit.getPosition(), targetUnit.getUnitID() };
			if(healAction.validate(state))
			{
				actionBucket.emplace_back(healAction);
			}
		}
	}

	void TBSActionSpace::generateEndOfTurnActions(TBSGameState& state, int playerID, std::vector<TBSAction>& actionBucket) const
	{
		TBSAction endTurnAction{ TBSActionType::EndTurn, playerID, -1, {}, -1 };
		if (endTurnAction.validate(state))
		{
			actionBucket.emplace_back(endTurnAction);
		}
	}
}