#include <Agent/ActionScripts/AttackClosestOpponentScript.h>

namespace SGA
{
	TBSAction AttackClosestOpponentScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
	{
		if (actionSpace.size() > 1)
		{
			std::vector<TBSAction> suitableActions;;
			for (const auto& action : actionSpace)
			{
				suitableActions.push_back(action);
			}

			std::vector<TBSUnit>& units = gameState.getUnits();
			std::map<int, Vector2i> positions = std::map<int, Vector2i>();
			std::set<int> opponentUnits = std::set<int>();

			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
				}
			}

			//prioritize attacks
			int actionDistance = std::numeric_limits<int>::max();
			int bestAction = -1;
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Attack && opponentUnits.contains(action.targetUnitID))
				{
					const int dist = positions[action.sourceUnitID].manhattanDistance(action.targetPosition);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
			}

			if (bestAction != -1)
				return suitableActions.at(bestAction);

			//moving closer to opponents
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = minimalDistanceToOpponents(action.targetPosition, positions, opponentUnits);
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

	TBSAction AttackClosestOpponentScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
		std::vector<TBSAction> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.sourceUnitID == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
		{
			int actionDistance = gameState.getBoard().getHeight() + gameState.getBoard().getWidth();
			int bestAction = -1;
			
			std::vector<TBSUnit>& units = gameState.getUnits();
			std::map<int, Vector2i> positions = std::map<int, Vector2i>();
			std::set<int> opponentUnits = std::set<int>();
			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
				}
			}
			
			//prioritize attacks
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Attack)
				{
					const int dist = positions[action.sourceUnitID].manhattanDistance(action.targetPosition);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
			}
			
			if (bestAction != -1)
				return suitableActions.at(bestAction);
			
			//moving closer to opponents
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = minimalDistanceToOpponents(action.targetPosition, positions, opponentUnits);
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

	int AttackClosestOpponentScript::minimalDistanceToOpponents(const Vector2i position,  std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits)
	{
		int minimalDistance = std::numeric_limits<int>::max();
		for (int opponentUnitID : opponentUnits)
		{
			const int distance = position.manhattanDistance(unitPositions[opponentUnitID]);
			if (distance < minimalDistance)
			{
				minimalDistance = distance;
			}
		}
		return minimalDistance;
	}
}
