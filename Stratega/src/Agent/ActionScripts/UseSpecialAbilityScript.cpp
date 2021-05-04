#include <Stratega/Agent/ActionScripts/UseSpecialAbilityScript.h>

namespace SGA
{
	Action UseSpecialAbilityScript::getAction(const GameState& gameState, std::vector<SGA::Action>& actionSpace) const
	{
		if (actionSpace.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (auto a : *gameState.gameInfo->actionTypes)
			{
				actionTypeIDToActionTypeString[a.first] = a.second.name;
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			// filter attack, move and endturn actions
			auto subsetActions = filterAllButThisActionType(actionSpace, "Attack", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "Move", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "EndTurn", actionTypeIDToActionTypeString);

			// if there is anything left, return a random action
			if (!subsetActions.empty())
				return subsetActions[rand() % subsetActions.size()];
		}
		
		// return a random action by default
		return actionSpace[rand() % actionSpace.size()];
	}

	Action UseSpecialAbilityScript::getActionForUnit(const GameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.targets[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}
		
		if (suitableActions.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (auto a : *gameState.gameInfo->actionTypes)
			{
				actionTypeIDToActionTypeString[a.first] = a.second.name;
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			
			auto subsetActions = filterAllButThisActionType(suitableActions, "Attack", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "Move", actionTypeIDToActionTypeString);
			subsetActions = filterAllButThisActionType(subsetActions, "EndTurn", actionTypeIDToActionTypeString);

			

			// if there is any special ability to use, return a random one, else default to any random action
			if (!subsetActions.empty())
				return subsetActions[rand() % subsetActions.size()];
		}
		
		
		return actionSpace[rand() % actionSpace.size()];
	}
}
