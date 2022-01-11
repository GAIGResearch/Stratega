#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct TBSActionsWidget : public SGAWidget
	{
		const GameState* state;

		std::vector<SGA::Action> actionsHumanPlayer;

		std::unordered_set<int> selectedEntities;

		//List of targets
		std::vector<SGA::ActionTarget> selectedTargets;

		//Selected action type
		int actionTypeSelected = -1;

		//Let GUI know the widget is waiting for a position or entity to be assigned
		bool waitingForPosition = false;
		bool waitingForEntity = false;
		ActionAssignment& temp;

		TBSActionsWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ActionAssignment& temp);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

		void mouseButtonPressed(const sf::Event& event) override;

		//Configuration
		bool removeSelectedEntities = false;

		void reset()
		{
			actionTypeSelected = -1;
			selectedTargets.clear();

			if (removeSelectedEntities)
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

		std::vector<SGA::Action> getWidgetResult(int playerID);

		//Get TargetTypes
		void getActionTarget(int playerID, const SGA::ActionType& actionType, std::vector<SGA::Action>& actionsToExecute);

		void getEntityType(int playerID, const SGA::ActionType& actionType);
		void getTechnologyType(int playerID, const SGA::ActionType& actionType, std::vector<SGA::Action>& actionsToExecute);
		void getPositionReference();
		void getEntityReference();

		//Verify action targets
		void verifyActionTargets(int playerID, std::vector<SGA::Action>& actionsToExecute);
		void verifyPlayerActionTargets(int playerID, std::vector<SGA::Action>& actionsToExecute, const SGA::ActionType& actionType, SGA::Action& newAction);
		void verifyEntityActionTargets(int playerID, std::vector<SGA::Action>& actionsToExecute, const SGA::ActionType& actionType, SGA::Action& newAction);

		//Get action type
		void getActionType(int playerID);
		void getPlayerPossibleActionTypes(int playerID, std::unordered_set<int>& actionTypes);
		void getEntityPossibleActionTypes(std::unordered_set<int>& actionTypes);

		// Assign to the list of selected targets a position target
		void assignPosition(SGA::Vector2f position);
		//Assign to the list of selected targets a entity
		void assignEntity(int entity);


		//Checks if the seletec entities still exist
		bool checkSelectedEntitiesAvailable();
	};
}