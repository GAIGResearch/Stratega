#include <Agent/ActionScripts/AttackClosestOpponentScript.h>

namespace SGA
{
	Action<Vector2i> AttackClosestOpponentScript::getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const
	{
		if (actionSpace->count() > 1)
		{
			std::vector<Action<Vector2i>> suitableActions;;
			for (const auto& action : *actionSpace)
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
				if (action.getType() == ActionType::Attack && opponentUnits.contains(action.getTargetUnitID()))
				{
					const int dist = positions[action.getSourceUnitID()].manhattanDistance(action.getTargetPosition());
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
				if (action.getType() == ActionType::Move)
				{
					const int dist = minimalDistanceToOpponents(action.getTargetPosition(), positions, opponentUnits);
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
		
		return actionSpace->getAction(rand() % actionSpace->count());
	}

	Action<Vector2i> AttackClosestOpponentScript::getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const
	{
		std::vector<Action<Vector2i>> suitableActions;
		for (const auto& action : *actionSpace)
		{
			if (action.getSourceUnitID() == unitID)
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
				if (action.getType() == ActionType::Attack)
				{
					const int dist = positions[action.getSourceUnitID()].manhattanDistance(action.getTargetPosition());
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
				if (action.getType() == ActionType::Move)
				{
					const int dist = minimalDistanceToOpponents(action.getTargetPosition(), positions, opponentUnits);
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

		return actionSpace->getAction(rand() % actionSpace->count());
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
