#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>

namespace SGA
{
	Action RunToFriendlyUnitsScript::getAction(const GameState& gameState, std::vector<SGA::Action>& actionSpace) const
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
			std::set<int> friendlyUnits;
			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));

				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
					friendlyUnits.insert(entity.id);
				}
			}


			//else, try to move closer to an opponent
			std::vector<Action> subActions = filterActionTypes(actionSpace, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int actionDistance = std::numeric_limits<int>::max();

				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const int dist = maximalDistanceToFriends(action.targets[1].getPosition(gameState), positions, friendlyUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[bestAction];
			}
		}
		
		return actionSpace[actionSpace.size()-1];
	}

	Action RunToFriendlyUnitsScript::getActionForUnit(const GameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.targets[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}
		
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
			std::set<int> friendlyUnits;
			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));

				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
					friendlyUnits.insert(entity.id);
				}
			}


			//else, try to move closer to an opponent
			std::vector<Action> subActions = filterActionTypes(suitableActions, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int actionDistance = std::numeric_limits<int>::max();

				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const int dist = maximalDistanceToFriends(action.targets[1].getPosition(gameState), positions, friendlyUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[bestAction];
			}
		}
		
		return actionSpace[actionSpace.size()-1];
	}

	double RunToFriendlyUnitsScript::maximalDistanceToFriends(const Vector2f position, std::map<int, Vector2f>& unitPositions, const std::set<int>& friendlyUnits)
	{
		double maximalDistance = std::numeric_limits<double>::min();
		for (int friendlyUnitID : friendlyUnits)
		{
			const double distance = position.manhattanDistance(unitPositions[friendlyUnitID]);
			if (distance > maximalDistance)
			{
				maximalDistance = distance;
			}
		}
		return maximalDistance;
	}
}
