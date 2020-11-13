#include <Agent/ActionScripts/AttackWeakestOpponentScript.h>

namespace SGA
{
	TBSAction AttackWeakestOpponentScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
	{
		if (actionSpace.size() > 1)
		{
			std::vector<TBSAction> suitableActions = std::vector<TBSAction>();
			for (const auto& action : actionSpace)
			{
				suitableActions.push_back(action);
			}

			std::vector<TBSUnit>& units = gameState.getUnits();
			std::map<int, Vector2i> positions = std::map<int, Vector2i>();
			std::set<int> opponentUnits = std::set<int>();
			int minimalHealth = std::numeric_limits<int>::max();
			int weakestUnitID = -1;

			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
					if (unit.getHealthh() < minimalHealth)
					{
						minimalHealth = unit.getHealthh();
						weakestUnitID = unit.getUnitID();
					}
				}
			}

			int bestAction = -1;
			//prioritize attacks against the weakest unit
			for (auto& action : suitableActions)
			{
				if (action.type == TBSActionType::Attack && weakestUnitID == action.targetUnitID)
				{
					return action;
				}
			}

			if (bestAction != -1)
				return suitableActions.at(bestAction);

			//moving closer to weakest unit
			int actionDistance = std::numeric_limits<int>::max();
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = action.targetPosition.manhattanDistance(positions[weakestUnitID]);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
			}

			if (bestAction != -1)
				return suitableActions.at(bestAction);

			return suitableActions.at(rand() % suitableActions.size());
		}

		return actionSpace.at(rand() % actionSpace.size());
	}

	TBSAction AttackWeakestOpponentScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
		// todo
		if (actionSpace.size() > 1)
		{
			std::vector<TBSAction> suitableActions;
			for (const auto& action : actionSpace)
			{
				if (action.sourceUnitID == unitID)
					suitableActions.push_back(action);
			}

			std::vector<TBSUnit>& units = gameState.getUnits();
			std::map<int, Vector2i> positions = std::map<int, Vector2i>();
			std::set<int> opponentUnits = std::set<int>();
			int minimalHealth = std::numeric_limits<int>::max();
			int weakestUnitID = -1;

			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
					if (unit.getHealthh() < minimalHealth)
					{
						minimalHealth = unit.getHealthh();
						weakestUnitID = unit.getUnitID();
					}
				}
			}

			int bestAction = -1;
			//prioritize attacks against the weakest unit
			for (auto& action : suitableActions)
			{
				if (action.type == TBSActionType::Attack && weakestUnitID == action.targetUnitID)
				{
					return action;
				}
			}

			if (bestAction != -1)
				return suitableActions.at(bestAction);

			//moving closer to weakest unit
			int actionDistance = std::numeric_limits<int>::max();
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = action.targetPosition.manhattanDistance(positions[weakestUnitID]);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
			}

			if (bestAction != -1)
				return suitableActions.at(bestAction);

			return suitableActions.at(rand() % suitableActions.size());
		}
		
		return actionSpace.at(rand() % actionSpace.size());
	}
}
