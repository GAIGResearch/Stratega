#include <Agent/ActionScripts/UseSpecialAbilityScript.h>

namespace SGA
{
	TBSAction UseSpecialAbilityScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
	{
		if (actionSpace.size() > 1)
		{
			std::vector<TBSAction> suitableActions;
			for (const auto& action : actionSpace)
			{
				if (action.type != TBSActionType::Attack && action.type != TBSActionType::Move && action.type != TBSActionType::EndTurn)
					suitableActions.push_back(action);
			}

			// if there is any special ability to use, return a random one, else default to any random action
			if (!suitableActions.empty())
				return suitableActions.at(rand() % suitableActions.size());
		}

		return actionSpace.at(rand() % actionSpace.size());
	}

	TBSAction UseSpecialAbilityScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
		if (actionSpace.size() > 1)
		{
			std::vector<TBSAction> suitableActions;
			for (const auto& action : actionSpace)
			{
				if (action.type != TBSActionType::Attack && action.type != TBSActionType::Move && action.type != TBSActionType::EndTurn
					&& action.sourceUnitID == unitID)
					suitableActions.push_back(action);
			}

			// if there is any special ability to use, return a random one, else default to any random action
			if (!suitableActions.empty())
				return suitableActions.at(rand() % suitableActions.size());
		}
		
		return actionSpace.at(rand() % actionSpace.size());
	}
}
