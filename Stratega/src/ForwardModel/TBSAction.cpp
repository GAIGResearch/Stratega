#include <ForwardModel/TBSAction.h>

namespace SGA
{
	bool TBSAction::validate(TBSGameState& state) const
	{
		switch (type)
		{
			case TBSActionType::Attack: return validateAttack(state);
			case TBSActionType::Move: return validateMove(state);
			case TBSActionType::Heal: return validateHeal(state);
			case TBSActionType::Push: return validatePush(state);
			case TBSActionType::EndTurn: return validateEndOfTurn(state);
			default: throw std::runtime_error("Tried validating an action with an not supported action-type");
		}
	}

	bool TBSAction::validateMove(TBSGameState& state) const
	{
		TBSUnit* unit = state.getUnit(sourceUnitID);
		if (unit == nullptr)
			return false;

		return state.isWalkable(targetPosition) && unit->getPosition().distance(targetPosition) <= unit->getRange();
	}
	
	bool TBSAction::validateAttack(TBSGameState& state) const
	{
		auto* attacker = state.getUnit(sourceUnitID);
		auto* target = state.getUnit(targetUnitID);
		if (attacker == nullptr || target == nullptr)
			return false;

		return attacker->getUnitID() != target->getUnitID() &&
			attacker->getPosition().manhattanDistance(target->getPosition()) <= attacker->getType().actionRange;
	}
	
	bool TBSAction::validatePush(TBSGameState& state) const
	{
		auto* pusher = state.getUnit(sourceUnitID);
		auto* target = state.getUnit(targetUnitID);
		if (pusher == nullptr || target == nullptr)
			return false;

		// We can only push in cardinal directions
		return pusher->getUnitID() != target->getUnitID() &&
			pusher->getPosition().manhattanDistance(target->getPosition()) == 1;
	}
	
	bool TBSAction::validateHeal(TBSGameState& state) const
	{
		auto* healer = state.getUnit(sourceUnitID);
		auto* target = state.getUnit(targetUnitID);
		if (healer == nullptr || target == nullptr)
			return false;

		return target->getHealth() < target->getType().maxHealth && healer->getPosition().manhattanDistance(target->getPosition()) <= healer->getType().actionRange;
	}
	
	bool TBSAction::validateEndOfTurn(TBSGameState& state) const
	{
		return true;
	}
}
