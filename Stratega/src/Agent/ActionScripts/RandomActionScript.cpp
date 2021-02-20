#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

namespace SGA
{
	Action RandomActionScript::getAction(TBSGameState& gameState, std::vector<Action>& actionSpace) const
	{
		return actionSpace[rand() % actionSpace.size()];
	}
	
	Action RandomActionScript::getActionForUnit(TBSGameState& gameState, std::vector<Action>& actionSpace, int unitID) const
	{
		std::vector<Action> suitableActions;
		/*for (const auto& action : actionSpace)
		{
			auto& actionType = gameState.getActionType(action.actionTypeID);
			if (actionType.sourceType == ActionSourceType::Unit)
			{
				auto sourceEntity = targetToEntity(gameState, action.targets[0]);
				if(sourceEntity.id == unitID)
					suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
			return suitableActions.at(rand() % suitableActions.size());*/
		
		return actionSpace[rand() % actionSpace.size()];
	}
}
