#include <Stratega/Agent/ActionScripts/RandomActionScript.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#pragma warning(disable: 5045)
namespace SGA
{
	Action RandomActionScript::getAction(const GameState& /*gameState*/, std::vector<Action>& actionSpace, int playerID) const
	{
		/*if (actionSpace.size() > 0)
			return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
		else 
			return Action::createEndAction(playerID);	
		*/
		return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
	}
	
	Action RandomActionScript::getActionForUnit(const GameState& /*gameState*/, std::vector<Action>& actionSpace, int playerID, int /*unitID*/) const
	{
		//std::vector<Action> suitableActions;
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

		if (actionSpace.size() > 0)
			return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
		else return Action::createEndAction(playerID);
	}
}
