#include <ForwardModel/RTSAction.h>
#include <Representation/RTSGameState.h>

namespace SGA
{
	bool RTSAction::validate(RTSGameState& state) const
	{
		switch(type)
		{
			case RTSActionType::Move: return validateMove(state);
			case RTSActionType::Attack: return validateAttack(state);
			case RTSActionType::Heal: return validateHeal(state);
			default: return true;
		}
	}
	
	bool RTSAction::validateMove(RTSGameState& state) const
	{
		return state.getUnit(sourceUnitID) != nullptr;
	}

	bool RTSAction::validateAttack(RTSGameState& state) const
	{
		auto* attacker = state.getUnit(sourceUnitID);
		auto* target = state.getUnit(targetUnitID);
		if (attacker == nullptr || target == nullptr)
			return false;

		//We dont need to validate the distance because the unit will aprroach to the target
		//return attacker->unitID != target->unitID && attacker->actionCooldown <= 0 && attacker->position.distance(target->position) <= attacker->actionRange;
		return attacker->unitID != target->unitID && attacker->playerID != target->playerID/* && attacker->position.distance(target->position) <= attacker->actionRange*/;
	}

	bool RTSAction::validateHeal(RTSGameState& state) const
	{
		auto* healer = state.getUnit(sourceUnitID);
		auto* target = state.getUnit(targetUnitID);
		if (healer == nullptr || target == nullptr)
			return false;

		return healer->actionCooldown <= 0 && target->health < target->maxHealth&& healer->position.distance(target->position) <= healer->actionRange;
	}
}