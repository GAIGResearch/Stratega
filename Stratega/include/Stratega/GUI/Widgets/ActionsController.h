#pragma once
#include <unordered_set>
#include <vector>

//Forward declaration
namespace SGA
{
	struct Action;
	struct ActionType;
	class ActionTarget;
	struct GameState;
	struct Vector2f;
}


namespace SGA::Widgets
{
	struct ActionsSettings
	{
		std::vector<Action> actionsHumanPlayer;

		std::unordered_set<int> selectedEntities;
		//List of targets
		std::vector<ActionTarget> selectedTargets;

		//Selected action type
		int actionTypeSelected = -1;		

		//Let GUI know the widget is waiting for a position or entity to be assigned
		bool waitingForPosition = false;
		bool waitingForEntity = false;

		//Configuration
		bool removeSelectedEntities = true;
		
		void reset()
		{
			actionTypeSelected = -1;
			selectedTargets.clear();

			if(removeSelectedEntities)
				actionsHumanPlayer.clear();
			
			selectedEntities.clear();
			waitingForPosition = false;
			waitingForEntity = false;
		}

		bool hasActionsTargetAvailable(const ActionType& actionType) const;

		bool hasActionTypeSelected() const
		{
			return actionTypeSelected != -1;
		}

		bool hasEntitiesSelected() const
		{
			return !selectedEntities.empty();
		}

		bool isSelected(int unitID)
		{
			return selectedEntities.find(unitID) != selectedEntities.end();
		}
	};
	
	/// <summary>
	/// Displays a widget for selecting the action
	/// </summary>
	/// <param name="state">The current GameState</param>
	/// <param name="settings">The settings that will use to draw the possible actions</param>
	/// <returns> The action result to execute</returns>
	std::vector<Action> getWidgetResult(GameState& state, ActionsSettings& settings, int playerID);

	//Get TargetTypes
	void getActionTarget(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute);

	void getEntityType(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType);
	void getTechnologyType(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute);
	void getPositionReference(ActionsSettings& settings);
	void getEntityReference(ActionsSettings& settings);
	
	//Verify action targets
	void verifyActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute);
	void verifyPlayerActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction);
	void verifyEntityActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction);
	
	//Get action type
	void getActionType(GameState& state, ActionsSettings& settings, int playerID);
	void getPlayerPossibleActionTypes(GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes);
	void getEntityPossibleActionTypes(GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes);
	
	// Assign to the list of selected targets a position target
	void assignPosition(const GameState& state, ActionsSettings& settings, Vector2f position);
	//Assign to the list of selected targets a entity
	void assignEntity(const GameState& state, ActionsSettings& settings, int entity);

}
