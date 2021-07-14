#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/ForwardModel/ForwardModel.h>

namespace SGA
{
	
	
	Action AttackClosestOpponentScript::getAction(const GameState& gameState, std::vector<Action>& actionSpace, int playerID) const
	{
		if (actionSpace.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto a : *gameState.getGameInfo()->actionTypes)
			{
				actionTypeIDToActionTypeString[a.first] = a.second.getName();
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			
			// store positions of all entities and a set of opponent ids
			std::map<int, Vector2f> positions = std::map<int, Vector2f>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			for (auto& entity : gameState.getEntities()) {
				positions.insert(std::pair<int, Vector2f>(entity.getID(), entity.getPosition()));

				if (entity.getOwnerID() == playerID)
				{
					myUnits.push_back(entity.getID());
				}
				else
				{
					opponentUnits.insert(entity.getID());
				}
			}
		

			//prioritize attacks, if at least one attack action targeting an opponent entity is available, return the attack action to the closest opponent
			double actionDistance = std::numeric_limits<int>::max();
			auto subActions = filterActionTypes(actionSpace, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					
					if (opponentUnits.contains(action.targets[1].getEntityID()))
					{
						const double dist = positions[action.targets[0].getEntityID()].manhattanDistance(positions[action.targets[1].getEntityID()]);
						if (dist < actionDistance)
						{
							actionDistance = dist;
								bestAction = i;
						}
					}
				}
				if (bestAction != -1)
					return subActions[bestAction];
			}
			
			
			//else, try to move closer to an opponent
			subActions = filterActionTypes(actionSpace, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = minimalDistanceToOpponents(action.targets[1].getPosition(gameState), positions, opponentUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
				
				if (bestAction != -1)
					return subActions[bestAction];
			}

			
			//else, try to do something else than attacking yourself
			subActions = filterAllButThisActionType(actionSpace, "Attack", actionTypeIDToActionTypeString);
			if (subActions.size() > 0)
			{
				return subActions[rand() % subActions.size()];
			}

		}
		
		// if nothing has been returned so far, return a random action
		return actionSpace[rand() % actionSpace.size()];
	}

	Action AttackClosestOpponentScript::getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int playerID, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.targets[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto a : *gameState.getGameInfo()->actionTypes)
			{
				actionTypeIDToActionTypeString[a.first] = a.second.getName();
			}
			actionTypeIDToActionTypeString[-1] = "EndTurn";

			
			// store positions of all entities and a set of opponent ids
			std::map<int, Vector2f> positions = std::map<int, Vector2f>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			for (auto& entity : gameState.getEntities()) {
				positions.insert(std::pair<int, Vector2f>(entity.getID(), entity.getPosition()));

				if (entity.getOwnerID() == playerID)
				{
					myUnits.push_back(entity.getID());
				}
				else
				{
					opponentUnits.insert(entity.getID());
				}
			}
			
			//prioritize attacks, if at least one attack action targeting an opponent entity is available, return the attack action to the closest opponent
			double actionDistance = std::numeric_limits<int>::max();
			auto subActions = filterActionTypes(suitableActions, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);

					if (opponentUnits.contains(action.targets[1].getEntityID()))
					{
						const double dist = action.targets[0].getPosition(gameState).manhattanDistance(positions[action.targets[1].getEntityID()]);
						if (dist < actionDistance)
						{
							actionDistance = dist;
							bestAction = i;
						}
					}
				}
				if (bestAction != -1)
					return subActions[bestAction];
			}


			//else, try to move closer to an opponent
			subActions = filterActionTypes(suitableActions, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = minimalDistanceToOpponents(positions[action.targets[1].getEntityID()], positions, opponentUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[bestAction];
			}


			//else, try to do something else than attacking yourself
			subActions = filterAllButThisActionType(suitableActions, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				return subActions[rand() % subActions.size()];
			}
		}

		return actionSpace[rand() % actionSpace.size()];
	}

	double AttackClosestOpponentScript::minimalDistanceToOpponents(const Vector2f position,  std::map<int, Vector2f>& unitPositions, const std::set<int>& opponentUnits)
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
