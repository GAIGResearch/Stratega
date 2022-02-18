#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <set>
#include <Stratega/Utils/contains.h>
#pragma warning(disable: 5045)
namespace SGA
{

	double AttackWeakestOpponentScript::getHealth(const Entity& entity, const GameState& gamestate) const
	{
		for (const auto& param : (gamestate.getGameInfo()->getEntityTypes()).at(entity.getEntityTypeID()).getParameters())
		{
			if (param.second.getName() == "Health")
			{
				return entity.getParamValues().find(param.second.getIndex())->second;
			}
		}
		return 0;
	}
	
	Action AttackWeakestOpponentScript::getAction(const GameState& gameState, std::vector<Action>& actionSpace, int playerID) const
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
			std::map<int, double> healthPerUnit = std::map<int, double>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			double minimalHealth = std::numeric_limits<double>::max();
			int weakestUnitID = -1;

			for (auto& entity : gameState.getEntities()) {
				positions.insert(std::pair<int, Vector2f>(entity.getID(), entity.getPosition()));
				double health =  getHealth(entity, gameState);
				healthPerUnit.insert(std::pair<int, double>(entity.getID(), health));

				if (health < minimalHealth)
				{
					minimalHealth = health;
					weakestUnitID = entity.getID();
				}
				
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
			double minimalTargetHealthPoints = std::numeric_limits<double>::max();
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
						const double health = healthPerUnit[action.getTargets()[1].getEntityID()];
						if (health < minimalTargetHealthPoints)
						{
							minimalTargetHealthPoints = health;
							bestAction = static_cast<int>(i);
						}
					}
				}
				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
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
					const double dist = action.getTargets()[1].getPosition(gameState).manhattanDistance(positions[weakestUnitID]);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}
		}
		
		return actionSpace[actionSpace.size() - 1];
	}

	Action AttackWeakestOpponentScript::getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int playerID, int unitID) const
	{
		std::vector<Action> suitableActions;
		for (const auto& action : actionSpace)
		{
			if (action.getTargets()[0].getEntityID() == unitID)
			{
				suitableActions.push_back(action);
			}
		}
		
		if (suitableActions.size() > 1)
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
			std::map<int, double> healthPerUnit = std::map<int, double>();
			std::vector<int> myUnits;
			std::set<int> opponentUnits;
			double minimalHealth = std::numeric_limits<double>::max();
			int weakestUnitID = -1;

			for (auto& entity : gameState.getEntities()) {
				positions.insert(std::pair<int, Vector2f>(entity.getID(), entity.getPosition()));
				double health = getHealth(entity, gameState);
				healthPerUnit.insert(std::pair<int, double>(entity.getID(), health));

				if (health < minimalHealth)
				{
					minimalHealth = health;
					weakestUnitID = entity.getID();
				}

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
			double minimalTargetHealthPoints = std::numeric_limits<double>::max();
			auto subActions = filterActionTypes(suitableActions, "Attack", actionTypeIDToActionTypeString);
			if (!subActions.empty())
			{
				int bestAction = -1;
				for (size_t i = 0; i < subActions.size(); i++)
				{
					auto& action = subActions.at(i);

					if (opponentUnits.count(action.getTargets()[1].getEntityID()))
					//if (opponentUnits.contains(action.getTargets()[1].getEntityID()))
					{
						const double health = healthPerUnit[action.getTargets()[1].getEntityID()];
						if (health < minimalTargetHealthPoints)
						{
							minimalTargetHealthPoints = health;
							bestAction = static_cast<int>(i);
						}
					}
				}
				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
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
					const double dist = action.getTargets()[1].getPosition(gameState).manhattanDistance(positions[weakestUnitID]);
					if (dist < actionDistance)
					{
						actionDistance = dist;
						bestAction = static_cast<int>(i);
					}
				}

				if (bestAction != -1)
					return subActions[static_cast<size_t>(bestAction)];
			}
		}
		return actionSpace[static_cast<size_t>(static_cast<int>(actionSpace.size())-1)];
	}
}
