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


namespace Widgets
{
	struct ActionsSettings
	{
		std::vector<SGA::Action> actionsHumanPlayer;

		std::unordered_set<int> selectedEntities;
		//List of targets
		std::vector<SGA::ActionTarget> selectedTargets;

		//Selected action type
		int actionTypeSelected = -1;		

		//Let GUI know the widget is waiting for a position or entity to be assigned
		bool waitingForPosition = false;
		bool waitingForEntity = false;

		//Configuration
		bool removeSelectedEntities = false;
		
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

		bool hasActionsTargetAvailable(const SGA::ActionType& actionType) const;

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
	std::vector<SGA::Action> getWidgetResult(SGA::GameState& state, ActionsSettings& settings, int playerID);

	//Get TargetTypes
	void getActionTarget(SGA::GameState& state, ActionsSettings& settings, int playerID, const SGA::ActionType& actionType, std::vector<SGA::Action>& actionsToExecute);

	void getEntityType(SGA::GameState& state, ActionsSettings& settings, int playerID, const SGA::ActionType& actionType);
	void getTechnologyType(SGA::GameState& state, ActionsSettings& settings, int playerID, const SGA::ActionType& actionType, std::vector<SGA::Action>& actionsToExecute);
	void getPositionReference(ActionsSettings& settings);
	void getEntityReference(ActionsSettings& settings);
	
	//Verify action targets
	void verifyActionTargets(SGA::GameState& state, ActionsSettings& settings, int playerID, std::vector<SGA::Action>& actionsToExecute);
	void verifyPlayerActionTargets(SGA::GameState& state, ActionsSettings& settings, int playerID, std::vector<SGA::Action>& actionsToExecute, const SGA::ActionType& actionType, SGA::Action& newAction);
	void verifyEntityActionTargets(SGA::GameState& state, ActionsSettings& settings, int playerID, std::vector<SGA::Action>& actionsToExecute, const SGA::ActionType& actionType, SGA::Action& newAction);
	
	//Get action type
	void getActionType(SGA::GameState& state, ActionsSettings& settings, int playerID);
	void getPlayerPossibleActionTypes(SGA::GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes);
	void getEntityPossibleActionTypes(SGA::GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes);
	
	// Assign to the list of selected targets a position target
	void assignPosition(const SGA::GameState& state, ActionsSettings& settings, SGA::Vector2f position);
	//Assign to the list of selected targets a entity
	void assignEntity(const SGA::GameState& state, ActionsSettings& settings, int entity);


	//Checks if the seletec entities still exist
	bool checkSelectedEntitiesAvailable(SGA::GameState& state, ActionsSettings& settings);
}
