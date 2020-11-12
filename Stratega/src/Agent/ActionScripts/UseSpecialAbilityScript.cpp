#include <Agent/ActionScripts/UseSpecialAbilityScript.h>

namespace SGA
{
	Action<Vector2i> UseSpecialAbilityScript::getAction(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace) const
	{
		if (actionSpace->size() > 1)
		{
			std::vector<Action<Vector2i>> suitableActions;
			for (const auto& action : *actionSpace)
			{
				if (action.getType() != ActionType::Attack && action.getType() != ActionType::Move && action.getType() != ActionType::EndTurn)
					suitableActions.push_back(action);
			}

			// if there is any special ability to use, return a random one, else default to any random action
			if (!suitableActions.empty())
				return suitableActions.at(rand() % suitableActions.size());
		}

		return actionSpace->at(rand() % actionSpace->size());
	}

	Action<Vector2i> UseSpecialAbilityScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace, int unitID) const
	{
		if (actionSpace->size() > 1)
		{
			std::vector<Action<Vector2i>> suitableActions;
			for (const auto& action : *actionSpace)
			{
				if (action.getType() != ActionType::Attack && action.getType() != ActionType::Move && action.getType() != ActionType::EndTurn
					&& action.getSourceUnitID() == unitID)
					suitableActions.push_back(action);
			}

			// if there is any special ability to use, return a random one, else default to any random action
			if (!suitableActions.empty())
				return suitableActions.at(rand() % suitableActions.size());
		}
		
		return actionSpace->at(rand() % actionSpace->size());
	}
}
