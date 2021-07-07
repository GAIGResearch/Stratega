#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <set>

namespace SGA
{

	double AttackWeakestOpponentScript::getHealth(const Entity* entity, const GameState& gamestate) const
	{
		for (const auto& param : (*gamestate.gameInfo->entityTypes)[entity->id].parameters)
		{
			if (param.second.name == "Health")
			{
				return entity->parameters[param.second.index];
			}
		}
		return 0;
	}
	
	Action AttackWeakestOpponentScript::getAction(const GameState& gameState, std::vector<SGA::Action>& actionSpace) const
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
			std::map<int, double> healthPerUnit = std::map<int, double>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			double minimalHealth = std::numeric_limits<double>::max();
			int weakestUnitID = -1;

			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));
				double health =  getHealth(&entity, gameState);
				healthPerUnit.insert(std::pair<int, double>(entity.id, health));

				if (health < minimalHealth)
				{
					minimalHealth = health;
					weakestUnitID = entity.id;
				}
				
				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
				}
				else
				{
					opponentUnits.insert(entity.id);
				}
			}


			//prioritize attacks, if at least one attack action targeting an opponent entity is available, return the attack action to the closest opponent
			double minimalTargetHealthPoints = std::numeric_limits<double>::max();
			auto subActions = filterActionTypes(actionSpace, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);

					if (opponentUnits.contains(action.targets[1].getEntityID()))
					{
						const double health = healthPerUnit[action.targets[1].getEntityID()];
						if (health < minimalTargetHealthPoints)
						{
							minimalTargetHealthPoints = health;
							bestAction = i;
						}
					}
				}
				if (bestAction != -1)
					return subActions[bestAction];
			}
			
			//else, try to move closer to the weakest opponent
			subActions = filterActionTypes(actionSpace, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				double actionDistance = std::numeric_limits<double>::max();
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = action.targets[1].getPosition(gameState).manhattanDistance(positions[weakestUnitID]);
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
		
		return actionSpace[actionSpace.size() - 1];
	}

	Action AttackWeakestOpponentScript::getActionForUnit(const GameState& gameState, std::vector<SGA::Action>& actionSpace, int unitID) const
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
			std::map<int, double> healthPerUnit = std::map<int, double>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			double minimalHealth = std::numeric_limits<double>::max();
			int weakestUnitID = -1;

			for (auto& entity : gameState.entities) {
				positions.insert(std::pair<int, Vector2f>(entity.id, entity.position));
				double health = getHealth(&entity, gameState);
				healthPerUnit.insert(std::pair<int, double>(entity.id, health));

				if (health < minimalHealth)
				{
					minimalHealth = health;
					weakestUnitID = entity.id;
				}

				if (entity.ownerID == gameState.currentPlayer)
				{
					myUnits.push_back(entity.id);
				}
				else
				{
					opponentUnits.insert(entity.id);
				}
			}


			//prioritize attacks, if at least one attack action targeting an opponent entity is available, return the attack action to the closest opponent
			double minimalTargetHealthPoints = std::numeric_limits<double>::max();
			auto subActions = filterActionTypes(suitableActions, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);

					if (opponentUnits.contains(action.targets[1].getEntityID()))
					{
						const double health = healthPerUnit[action.targets[1].getEntityID()];
						if (health < minimalTargetHealthPoints)
						{
							minimalTargetHealthPoints = health;
							bestAction = i;
						}
					}
				}
				if (bestAction != -1)
					return subActions[bestAction];
			}

			//else, try to move closer to the weakest opponent
			subActions = filterActionTypes(suitableActions, "Move", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				double actionDistance = std::numeric_limits<double>::max();
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);
					const double dist = action.targets[1].getPosition(gameState).manhattanDistance(positions[weakestUnitID]);
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
}
