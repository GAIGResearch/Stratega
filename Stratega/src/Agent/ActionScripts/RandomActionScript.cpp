#include <Agent/ActionScripts/RandomActionScript.h>

namespace SGA
{
	Action<Vector2i> RandomActionScript::getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const
	{
		return actionSpace->at(rand() % actionSpace->size());
	}
	
	Action<Vector2i> RandomActionScript::getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const
	{
		std::vector<Action<Vector2i>> suitableActions = std::vector<Action<Vector2i>>();
		for (const auto& action : *actionSpace)
		{
			if (action.getSourceUnitID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
			return suitableActions.at(rand() % suitableActions.size());
		
		return actionSpace->at(rand() % actionSpace->size());
	}
}
