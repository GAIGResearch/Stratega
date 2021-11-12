#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameState.h>
#pragma warning(disable: 5045)
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

	void Player::recomputeStats(GameState& state)
	{
		//Remove buffs applied but keep value clamped to min and max
		for (size_t i = 0; i < parameters.size(); i++)
		{
			const auto& param = state.getGameInfo()->getPlayerParameterByIndex(static_cast<int>(i));
			double maxParameter = param.getMaxValue();
			double minParameter = param.getMinValue();

			//Update the max value
			maxParameters[i] = maxParameter;

			//Keep parameter inside max and min
			if (parameters[i] > maxParameter)
				parameters[i] = maxParameter;
			else if (parameters[i] < minParameter)
				parameters[i] = minParameter;

		}

		//Recompute each parameter
		for (size_t i = 0; i < parameters.size(); i++)
		{
			const auto& param = state.getGameInfo()->getPlayerParameterByIndex(static_cast<int>(i));
			double previousMaxParameter = param.getMaxValue();
			double maxParameter = previousMaxParameter;

			//Add to each parameterMax the additive and multiplication:	
			//Add buffs additive
			for (auto& buff : buffs)
			{
				const auto& buffType = buff.getType();
				maxParameter = buffType.getParameterWithAdditiveBuffsApplied(maxParameter, param.getID());
			}

			double multiplicationSum = 0;
			//Add buffs multiplication
			for (auto& buff : buffs)
			{
				auto& buffType = buff.getType();
				multiplicationSum += buffType.getMultiplicationSum(previousMaxParameter, param.getID());
			}
			maxParameter += multiplicationSum;

			//Write new value with the different of the max parameters
			parameters[i] += maxParameter - previousMaxParameter;
			//Update the max value
			maxParameters[i] = maxParameter;
		}
	}
}
