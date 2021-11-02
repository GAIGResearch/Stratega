#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/Gamestate.h>
namespace SGA
{
	bool Player::canExecuteAction(int actionTypeID) const
	{
		for (const auto& actionType : attachedActions)
		{
			if (actionType.actionTypeID == actionTypeID)
				return true;
		}
		return false;
	}

	const ActionInfo& Player::getActionInfo(int actionTypeID) const
	{
		for (const auto& actionInfo : attachedActions)
		{
			if (actionInfo.actionTypeID == actionTypeID)
				return actionInfo;
		}

		throw std::runtime_error("Tried accessing action with unknown actionType");
	}

	void Player::removeBuffs(GameState& state)
	{
		//Recompute each parameter
		for (size_t i = 0; i < (size_t)parameters.size(); i++)
		{
			const auto& param = state.getGameInfo()->getPlayerParameterByIndex(i);

			double value = parameters[i];

			//Remove buffs multiplication
			for (auto& buff : buffs)
			{
				auto& buffType = buff.getType();
				value = buffType.getParameterWithOutMultiplicationBuffsApplied(value, param.getID());
			}

			// Add buffs additive
			for (auto& buff : buffs) {
				const auto& buffType = buff.getType();
				value = buffType.getParameterWithOutAdditiveBuffsApplied(value, param.getID());
			}

			//TODO: Check value is not over max or min values

			//Write new value with buffs applied
			parameters[i] = value;
		}
	}

	void Player::addBuffs(GameState& state)
	{
		//Recompute each parameter
		for (size_t i = 0; i < (size_t)parameters.size(); i++)
		{

			const auto& param = state.getGameInfo()->getPlayerParameterByIndex(i);
			double value = parameters[i];

			//Add buffs additive
			for (auto& buff : buffs)
			{
				const auto& buffType = buff.getType();
				value = buffType.getParameterWithAdditiveBuffsApplied(value, param.getID());
			}

			//Add buffs multiplication
			for (auto& buff : buffs)
			{
				auto& buffType = buff.getType();
				value = buffType.getParameterWithMultiplicationBuffsApplied(value, param.getID());
			}

			//TODO: Check value is not over max or min values

			//Write new value with buffs applied
			parameters[i] = value;
		}
	}
}
