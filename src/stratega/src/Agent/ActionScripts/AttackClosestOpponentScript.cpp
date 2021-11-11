#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Utils/contains.h>
#pragma warning(disable: 5045)
namespace SGA
{
	
	
	Action AttackClosestOpponentScript::getAction(const GameState& gameState, std::vector<Action>& actionSpace, int playerID) const
	{
		if (actionSpace.size() > 1)
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto& a : gameState.getGameInfo()->getActionTypes())
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

					const bool contains = opponentUnits.find(action.getTargets()[1].getEntityID()) != opponentUnits.end();

					if (contains)
					//if (contains(opponentUnits, action.getTargets()[1].getEntityID()))
					//if (opponentUnits.contains(action.getTargets()[1].getEntityID()))
					{
						const double dist = positions[action.getTargets()[0].getEntityID()].manhattanDistance(positions[action.getTargets()[1].getEntityID()]);
						if (dist < actionDistance)
						{
							actionDistance = dist;
								bestAction = static_cast<int>(i);
						}
					}
				}
				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}
			
			
			//else, try to move closer to an opponent
			subActions = filterActionTypes(actionSpace, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = minimalDistanceToOpponents(action.getTargets()[1].getPosition(gameState), positions, opponentUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}
				
				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}

			
			//else, try to do something else than attacking yourself
			subActions = filterAllButThisActionType(actionSpace, "Attack", actionTypeIDToActionTypeString);
			if (subActions.size() > 0)
			{
				return subActions[static_cast<size_t>(rand() % static_cast<int>(subActions.size()))];
			}

		}
		
		// if nothing has been returned so far, return a random action
		return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
	}

	Action AttackClosestOpponentScript::getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int playerID, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.getTargets()[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}

		if (!suitableActions.empty())
		{
			// create a map of action types to filter relevant actions
			std::map<int, std::string> actionTypeIDToActionTypeString;
			for (const auto& a : gameState.getGameInfo()->getActionTypes())
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

					const bool contains = opponentUnits.find(action.getTargets()[1].getEntityID()) != opponentUnits.end();

					if (contains)
					//if (contains(opponentUnits, action.getTargets()[1].getEntityID()))
					//if (opponentUnits.contains(action.getTargets()[1].getEntityID()))
					{
						const double dist = action.getTargets()[0].getPosition(gameState).manhattanDistance(positions[action.getTargets()[1].getEntityID()]);
						if (dist < actionDistance)
						{
							actionDistance = dist;
							bestAction = static_cast<int>(i);
						}
					}
				}
				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}


			//else, try to move closer to an opponent
			subActions = filterActionTypes(suitableActions, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = minimalDistanceToOpponents(positions[action.getTargets()[1].getEntityID()], positions, opponentUnits);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}


			//else, try to do something else than attacking yourself
			subActions = filterAllButThisActionType(suitableActions, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				return subActions[static_cast<size_t>(rand() % static_cast<int>(subActions.size()))];
			}
		}

		return actionSpace[static_cast<size_t>(rand() % static_cast<int>(actionSpace.size()))];
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
