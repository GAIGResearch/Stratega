#include <Agent/ActionScripts/RandomActionScript.h>

namespace SGA
{
	TBSAction RandomActionScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
	{
		return actionSpace[rand() % actionSpace.size()];
	}
	
	TBSAction RandomActionScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
		std::vector<TBSAction> suitableActions = std::vector<TBSAction>();
		for (const auto& action : actionSpace)
		{
			if (action.sourceUnitID == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
			return suitableActions.at(rand() % suitableActions.size());
		
		return actionSpace[rand() % actionSpace.size()];
	}
}
