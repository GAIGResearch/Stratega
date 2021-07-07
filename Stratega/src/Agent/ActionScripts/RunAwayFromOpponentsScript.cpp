#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>

namespace SGA
{
	Action RunAwayFromOpponentScript::getAction(const GameState& gameState, std::vector<SGA::Action>& actionSpace) const
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


			// store positions of all entities and a set of opponent ids
			std::map<int, Vector2f> positions = std::map<int, Vector2f>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));

				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
				}
				else
				{
					opponentUnits.insert(entity.id);
				}
			}

			
			//else, try to move closer to an opponent
			std::vector<Action> subActions = filterActionTypes(actionSpace, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int actionDistance = std::numeric_limits<int>::min();

				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const int dist = minimalDistanceToOpponents(action.targets[1].getPosition(gameState), positions, opponentUnits);
					if (dist > actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[bestAction];
			}
		}
		// else return endTurn Action
		return actionSpace[actionSpace.size()-1];
	}

	Action RunAwayFromOpponentScript::getActionForUnit(const GameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
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


			// store positions of all entities and a set of opponent ids
			std::map<int, Vector2f> positions = std::map<int, Vector2f>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));

				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
				}
				else
				{
					opponentUnits.insert(entity.id);
				}
			}


			//else, try to move closer to an opponent
			std::vector<Action> subActions = filterActionTypes(suitableActions, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int actionDistance = std::numeric_limits<int>::min();

				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const int dist = minimalDistanceToOpponents(action.targets[1].getPosition(gameState), positions, opponentUnits);
					if (dist > actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[bestAction];
			}
		}
		
		// else return endTurn Action
		return actionSpace[actionSpace.size()-1];
	}


	double RunAwayFromOpponentScript::minimalDistanceToOpponents(const Vector2f position, std::map<int, Vector2f>& unitPositions, const std::set<int>& opponentUnits)
	{
		double minimalDistance = std::numeric_limits<double>::max();
		for (int opponentUnitID : opponentUnits)
		{
			const double distance = position.manhattanDistance(unitPositions[opponentUnitID]);
			if (distance < minimalDistance)
			{
				minimalDistance = distance;
			}
		}
		return minimalDistance;
	}
}
