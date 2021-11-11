#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#pragma warning(disable: 5045)
namespace SGA
{
	Action UseSpecialAbilityScript::getAction(const GameState& gameState, std::vector<Action>& actionSpace, int /*playerID*/) const
	{
		if (actionSpace.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto& a : gameState.getGameInfo()->getActionTypes())
			{
				actionTypeIDToActionTypeString[a.first] = a.second.getName();
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			// filter attack, move and endturn actions
			auto subsetActions = filterAllButThisActionType(actionSpace, "Attack", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "Move", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "EndTurn", actionTypeIDToActionTypeString);

			// if there is anything left, return a random action
			if (!subsetActions.empty())
				return subsetActions[static_cast<size_t>(rand() % static_cast<int>(subsetActions.size()))];
		}
		
		// return a random action by default
		return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
	}

	Action UseSpecialAbilityScript::getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int /*playerID*/, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.getTargets()[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}
		
		if (suitableActions.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto& a : gameState.getGameInfo()->getActionTypes())
			{
				actionTypeIDToActionTypeString[a.first] = a.second.getName();
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			
			auto subsetActions = filterAllButThisActionType(suitableActions, "Attack", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "Move", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "EndTurn", actionTypeIDToActionTypeString);

			

			// if there is any special ability to use, return a random one, else default to any random action
			if (!subsetActions.empty())
				return subsetActions[static_cast<size_t>(rand() % static_cast<int>(subsetActions.size()))];
		}
		
		
		return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
	}
}
