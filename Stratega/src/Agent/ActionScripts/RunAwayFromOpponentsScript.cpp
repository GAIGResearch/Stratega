#include <Agent/ActionScripts/RunAwayFromOpponentScript.h>

namespace SGA
{
	TBSAction RunAwayFromOpponentScript::getAction(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace) const
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

			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
				}
			}

			//fleeing from opponents
			int actionDistance = std::numeric_limits<int>::min();
			int bestAction = -1;
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = maximalDistanceToOpponents(action.targetPosition, positions, opponentUnits);
					if (dist > actionDistance)
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

	TBSAction RunAwayFromOpponentScript::getActionForUnit(TBSGameState& gameState, std::vector<SGA::TBSAction>& actionSpace, int unitID) const
	{
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

			for (const TBSUnit& unit : units)
			{
				positions.insert(std::pair<int, Vector2i>(unit.getUnitID(), unit.getPosition()));
				if (unit.getPlayerID() != gameState.currentPlayer)
				{
					opponentUnits.insert(unit.getUnitID());
				}
			}

			//fleeing from opponents
			int actionDistance = std::numeric_limits<int>::min();
			int bestAction = -1;
			for (size_t i = 0; i < suitableActions.size(); i++)
			{
				auto& action = suitableActions.at(i);
				if (action.type == TBSActionType::Move)
				{
					const int dist = maximalDistanceToOpponents(action.targetPosition, positions, opponentUnits);
					if (dist > actionDistance)
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

	int RunAwayFromOpponentScript::maximalDistanceToOpponents(const Vector2i position, std::map<int, Vector2i>& unitPositions, const std::set<int>& opponentUnits)
	{
		int maximalDistance = std::numeric_limits<int>::min();
		for (int opponentUnitID : opponentUnits)
		{
			const int distance = position.manhattanDistance(unitPositions[opponentUnitID]);
			if (distance > maximalDistance)
			{
				maximalDistance = distance;
			}
		}
		return maximalDistance;
	}
}
