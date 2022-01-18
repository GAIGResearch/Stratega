#include <Stratega/NewGUI/TBSActionsWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	TBSActionsWidget::TBSActionsWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm, ActionAssignment& temp, std::unordered_set<int>& newSelectedEntities, int& newPlayerID):
		SGAWidget(widgetName, newWindow, newWorld, fm),
		state(nullptr),
		temp(temp),
		selectedEntities(newSelectedEntities),
		playerID(newPlayerID)
	{
	}

	void TBSActionsWidget::update(const GameState& state)
	{
		this->state = &state;
		actionsHumanPlayer = fm->generateActions(state, playerID/*pointOfViewPlayerID*/);
	}

	void TBSActionsWidget::render(SGARenderTarget& renderTarget)
	{
		//getWidgetResult();

		ImGuiWindowFlags window_flags = 0;
		window_flags += ImGuiWindowFlags_NoTitleBar;
		window_flags += ImGuiWindowFlags_NoScrollbar;
		//window_flags += ImGuiWindowFlags_MenuBar;
		//window_flags+= ImGuiWindowFlags_NoMove;
		//window_flags += ImGuiWindowFlags_NoResize;
		window_flags += ImGuiWindowFlags_NoCollapse;
		window_flags += ImGuiWindowFlags_NoNav;
		//window_flags+= ImGuiWindowFlags_NoBackground;
		window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;


		// We specify a default position/size in case there's no data in the .ini file.
		// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
		ImGui::SetNextWindowPos(ImVec2((static_cast<float>(window.getSize().x) / 2.f), (static_cast<float>(window.getSize().y) / 1.1f)), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(0, 0));

		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Separator();

		ImGui::Text("Actions");

		//Ask widget to get
		if (playerID != -1/*pointOfViewPlayerID != NO_PLAYER_ID*/)
		{
			auto actionsToExecute = getWidgetResult(playerID);
			if (!actionsToExecute.empty())
			{
				temp.clear();
				temp.assignActionOrReplace(actionsToExecute.front());
				//selectedAction = actionsToExecute.front();
			}
		}

		ImGui::Separator();
		ImGui::End();


		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Available Actions");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		int index = 0;
		for (auto action : actionsHumanPlayer)
		{

			std::string actionInfo = std::to_string(index);
			if (action.getActionTypeID() == -1)
			{
				if (action.getActionFlag() == ActionFlag::AbortContinuousAction)
				{
					if (action.getTargets()[0].getType() == ActionTarget::EntityReference)
					{
						//We need to find the continues action name that will abort
						auto& sourceEntity = *state->getEntityConst(action.getTargets()[0].getEntityID());
						for (const auto& continueAction : sourceEntity.getContinuousActions())
						{
							if (continueAction.getContinuousActionID() == action.getContinuousActionID())
							{
								const ActionType& actionType = continueAction.getActionType();
								actionInfo += " Abort " + actionType.getName();
							}
						}
					}
					else
					{
						//We need to find the continues action name that will abort
						auto& sourcePlayer = action.getTargets()[0].getPlayerConst(*state);
						for (auto& continueAction : sourcePlayer.getContinuousActions())
						{
							if (continueAction.getContinuousActionID() == action.getContinuousActionID())
							{
								const ActionType& actionType = continueAction.getActionType();
								actionInfo += " Abort " + actionType.getName();
							}
						}
					}



				}
				else
					actionInfo += " SpecialAction";
			}
			else
			{
				const ActionType& actionType = action.getActionType();

				actionInfo += " " + actionType.getName();

				//TODO Clean this :D IS TEMPORAL
				for (auto& targetType : action.getTargets())
				{
					switch (targetType.getType())
					{
					case ActionTarget::Position:
						actionInfo += " x:" + std::to_string(static_cast<int>(targetType.getPosition(*state).x)) + ",y:" + std::to_string(static_cast<int>(targetType.getPosition(*state).y));
						break;
					case ActionTarget::EntityReference:
						actionInfo += state->getEntityConst(targetType.getEntityID())->getEntityType().getName();
						break;
					case ActionTarget::PlayerReference:
						actionInfo += " Player: " + std::to_string(0/*pointOfViewPlayerID*/);
						break;
					case ActionTarget::TechnologyReference:
						actionInfo += " Technology: " + state->getGameInfo()->getTechnologyTreeCollection().getTechnology(targetType.getTechnologyID()).name;
						break;
					case ActionTarget::EntityTypeReference:
						actionInfo += " Entity: " + targetType.getEntityType(*state).getName();
						break;
					case ActionTarget::ContinuousActionReference:
						break;
					case ActionTarget::TileTypeReference:
						break;
					}
				}

			}

			index++;

			if (ImGui::Button(actionInfo.c_str()))
			{
				temp.clear();
				temp.assignActionOrReplace(action);
				break;
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();


		//Draw possible actions
		std::vector<sf::CircleShape> actionsShapes;
		if (waitingForPosition)
		{
			for (auto& possibleAction : actionsHumanPlayer)
			{
				//Check if action is compatible with the selected type and targets
				if (possibleAction.getActionTypeID() == -1 || possibleAction.getActionFlag() == ActionFlag::ContinuousAction
					|| possibleAction.getActionFlag() == ActionFlag::AbortContinuousAction
					|| possibleAction.getActionTypeID() != actionTypeSelected)
					continue;

				//Get source
				const auto& actionType = possibleAction.getActionType();
				//Check the source and the selected entity is the same
				if (actionType.getSourceType() == ActionSourceType::Entity)
				{
					auto& entity = possibleAction.getTargets()[0].getEntityConst(*state);
					if (entity.getID() != *selectedEntities.begin())
						continue;
				}

				for (auto& actionTarget : possibleAction.getTargets())
				{
					if (actionTarget.getType() == ActionTarget::Position)
					{
						auto position = actionTarget.getPosition(*state);

						sf::CircleShape possibleActionPositionShape(15);
						possibleActionPositionShape.setFillColor(sf::Color::White);
						possibleActionPositionShape.setOrigin(7.5, 7.5);
						sf::Vector2f temp = world.toSFML(position);
						possibleActionPositionShape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));

						actionsShapes.emplace_back(possibleActionPositionShape);
					}
				}

				for (const auto& shape : actionsShapes)
				{
					window.draw(shape);
				}
			}
		}
		else if (waitingForEntity)
		{
			for (auto& possibleAction : actionsHumanPlayer)
			{
				//Check if action is compatible with the selected type and targets
				if (possibleAction.getActionTypeID() == -1 || possibleAction.getActionFlag() == ActionFlag::ContinuousAction
					|| possibleAction.getActionFlag() == ActionFlag::AbortContinuousAction ||
					possibleAction.getActionTypeID() != actionTypeSelected)
					continue;

				//Get source
				const auto& actionType = possibleAction.getActionType();

				//Check the source and the selected entity is the same
				if (actionType.getSourceType() == ActionSourceType::Entity)
				{
					auto& entity = possibleAction.getTargets()[0].getEntityConst(*state);
					if (entity.getID() != *selectedEntities.begin())
						continue;
				}

				//Avoid source entity
				for (size_t i = 1; i < possibleAction.getTargets().size(); ++i)
				{
					if (possibleAction.getTargets()[i].getType() == ActionTarget::EntityReference)
					{
						auto position = possibleAction.getTargets()[i].getPosition(*state);

						sf::CircleShape possibleActionPositionShape(15);
						possibleActionPositionShape.setFillColor(sf::Color::White);
						possibleActionPositionShape.setOrigin(7.5, 7.5);
						sf::Vector2f temp = world.toSFML(position);
						possibleActionPositionShape.setPosition(temp + sf::Vector2f(TILE_OFFSET_ORIGIN_X, TILE_OFFSET_ORIGIN_Y));

						actionsShapes.emplace_back(possibleActionPositionShape);
					}
				}

				for (const auto& shape : actionsShapes)
				{
					window.draw(shape);
				}
			}
		}

	}


	void TBSActionsWidget::mouseButtonPressed(const sf::Event& event)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			Vector2i pos = world.toStrategaRounded(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
			//If selected unit we check if there is action in tile
			if (waitingForPosition)
			{
				assignPosition({ static_cast<float>(pos.x),static_cast<float>(pos.y) });
			}
			else
			{
				if (!waitingForEntity)
				{
					selectedEntities.clear();
					actionTypeSelected = -1;
				}
			}


			auto* selectedEntity = state->getEntityAt(Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)));
			if (selectedEntity /*&& playerID==0 *//*&& ((fowSettings.renderFogOfWar && (pointOfViewPlayerID == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar)*/)
			{
				//Assign selected unit
				if (waitingForEntity)
				{
					assignEntity(selectedEntity->getID());
				}
				else
				{
					//Pick up entity
					//if (selectedEntity->getOwnerID() == playerID)
						selectedEntities.emplace(selectedEntity->getID());
				}
			}
			//else
			//{
			//	//Restart selected actions of unit and selected unit
			//	dragging = true;
			//	oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			//}
		}
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			//selectedAction = Action::createEndAction(pointOfViewPlayerID);
			temp.clear();
			temp=temp.createEndActionAssignment(state->getCurrentTBSPlayer());
		}		
	}

	bool TBSActionsWidget::hasActionsTargetAvailable(const SGA::ActionType& actionType) const
	{
		return selectedTargets.size() < actionType.getTargets().size();
	}

	void TBSActionsWidget::getActionTarget(int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
	{
		//Draw target
		auto& targetType = actionType.getTargets()[selectedTargets.size()].first;
		switch (targetType.getType())
		{
		case TargetType::EntityType:
		{
			getEntityType(playerID, actionType);
			break;
		}
		case TargetType::Technology:
		{
			getTechnologyType(playerID, actionType, actionsToExecute);
			break;
		}
		case TargetType::Position:
		{
			getPositionReference();
			break;
		}
		case TargetType::Entity:
		{
			getEntityReference();
			break;
		}
		case TargetType::None:
		{
			break;
		}
		case TargetType::ContinuousAction:
		{
			break;
		}
		}

		ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0, 0.6f, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0., 0.7f, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0, 0.8f, 0.8f)));

		//Reset button
		if (ImGui::Button("Reset", ImVec2(50, 50)))
		{
			reset();
		}

		ImGui::PopStyleColor(3);
	}


	std::vector<Action> TBSActionsWidget::getWidgetResult(int playerID)
	{
		std::vector<Action> actionsToExecute;
		//Check if the we have action type selected
		if (hasActionTypeSelected())
		{
			//Selected actiontype
			const ActionType& actionType = state->getGameInfo()->getActionType(actionTypeSelected);

			//Check if we have selected each action target
			if (hasActionsTargetAvailable(actionType))
			{
				getActionTarget(playerID, actionType, actionsToExecute);
			}
			else
			{
				verifyActionTargets(playerID, actionsToExecute);
			}
		}
		else
		{
			getActionType(playerID);
		}

		return actionsToExecute;
	}


	void TBSActionsWidget::getTechnologyType(int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
	{
		if (hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& possibleTechnology : actionType.getTargets()[selectedTargets.size()].first.getTechnologyTypes())
			{
				//If player is researching abort action
				for (auto& entityID : selectedEntities)
				{
					for (const auto& action : state->getEntityConst(entityID)->getContinuousActions())
					{
						if (action.getActionTypeID() == actionTypeSelected)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0, 0.6f, 0.6f)));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0., 0.7f, 0.7f)));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0, 0.8f, 0.8f)));
							if (ImGui::Button("Abort", ImVec2(50, 50)))
							{
								actionsToExecute.emplace_back(Action::createAbortEntityAction(playerID, state->getEntityConst(entityID)->getID(), action.getContinuousActionID()));
							}
							ImGui::PopStyleColor(3);

							goto  FINISH1;
						}
					}
				}

				//Check if the player can research the possible technologies
				if (!state->canResearch(playerID, possibleTechnology))
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state->getGameInfo()->getTechnologyTreeCollection().getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
				{
					//Check if the player fullfill the technology
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
					actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
					//Check if any of the selectedEntities fullfill the condition
					for (auto& entity : selectedEntities)
					{
						actionTargets[0] = (ActionTarget::createEntityActionTarget(entity));

						//Check if we fullfil the conditions to spawn the entity
						for (auto& condition : actionType.getTargets()[selectedTargets.size()].second)
						{
							if (!condition->isFullfiled(*state, actionTargets))
							{
								canExecute = false;
								break;
							}

						}
					}
					if (canExecute)
						selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();


			}
		FINISH1:
			return;
		}
		else
		{
			int elementNumber = 0;
			for (auto& possibleTechnology : actionType.getTargets()[selectedTargets.size()].first.getTechnologyTypes())
			{
				//If player is researching abort action
				for (auto& action : state->getPlayer(playerID)->getContinuousActions())
				{
					if (action.getActionTypeID() == actionTypeSelected)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0, 0.6f, 0.6f)));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0., 0.7f, 0.7f)));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0, 0.8f, 0.8f)));
						if (ImGui::Button("Abort", ImVec2(50, 50)))
						{
							actionsToExecute.emplace_back(Action::createAbortPlayerAction(playerID, action.getContinuousActionID()));
						}
						ImGui::PopStyleColor(3);

						goto  FINISH2;
					}
				}

				//Check if the player can research the possible technologies
				if (!state->canResearch(playerID, possibleTechnology))
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state->getGameInfo()->getTechnologyTreeCollection().getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
				{
					//Check if the player fullfill the technology
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
					actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));

					//Check if player fullfil the conditions to spawn the entity
					for (auto& condition : actionType.getTargets()[selectedTargets.size()].second)
					{
						if (!condition->isFullfiled(*state, actionTargets))
						{
							canExecute = false;
							break;
						}
					}

					if (canExecute)
						selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();


			}
		FINISH2:
			return;
		}
	}

	void TBSActionsWidget::getEntityType(int playerID, const ActionType& actionType)
	{
		if (hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& possibleActionType : actionType.getTargets()[selectedTargets.size()].first.getGroupEntityTypes())
			{
				bool canSpawn = true;

				//Check if any selectedEntity can spawn the targets
				for (auto& entity : selectedEntities)
				{
					auto& entityType = state->getEntityConst(entity)->getEntityType();
					if (entityType.getSpawnableEntityTypes().find(possibleActionType) == entityType.getSpawnableEntityTypes().end())
					{
						canSpawn = false;
					}
				}

				if (!canSpawn)
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state->getGameInfo()->getEntityType(possibleActionType).getName().c_str(), ImVec2(50, 50)))
				{
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
					actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

					//Check if any of the selectedEntities fullfill the condition
					for (auto& entity : selectedEntities)
					{
						actionTargets[0] = (ActionTarget::createEntityActionTarget(entity));

						//Check if we fullfil the conditions to spawn the entity
						for (auto& condition : actionType.getTargets()[selectedTargets.size()].second)
						{
							if (!condition->isFullfiled(*state, actionTargets))
							{
								canExecute = false;
								break;
							}
						}
					}
					if (canExecute)
						selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		else
		{
			int elementNumber = 0;
			for (auto& possibleActionType : actionType.getTargets()[selectedTargets.size()].first.getGroupEntityTypes())
			{
				bool canSpawn = true;

				if (state->getGameInfo()->getPlayerSpawnableTypes().find(possibleActionType) == state->getGameInfo()->getPlayerSpawnableTypes().end())
				{
					canSpawn = false;
				}

				if (!canSpawn)
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state->getGameInfo()->getEntityType(possibleActionType).getName().c_str(), ImVec2(50, 50)))
				{
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
					actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

					//Check if player fullfil the conditions to spawn the entity
					for (auto& condition : actionType.getTargets()[selectedTargets.size()].second)
					{
						if (!condition->isFullfiled(*state, actionTargets))
						{
							canExecute = false;
							break;
						}
					}

					if (canExecute)
						selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}
		}

	}

	void TBSActionsWidget::getPositionReference()
	{
		ImGui::Text("Choose position");
		//Need to receive a new position from the gui
		waitingForPosition = true;
	}

	void TBSActionsWidget::getEntityReference()
	{
		ImGui::Text("Choose entity");
		//Need to receive a new position from the gui
		waitingForEntity = true;
	}


	void TBSActionsWidget::verifyActionTargets(int playerID, std::vector<Action>& actionsToExecute)
	{
		//Verify the selected targets are valid						
		const ActionType& actionType = state->getGameInfo()->getActionType(actionTypeSelected);

		//Generate action with the current selected settings
		Action newAction(&actionType);

		if (actionType.isContinuous())
			newAction.setActionFlag(ActionFlag::ContinuousAction);

		if (selectedEntities.empty())
			verifyPlayerActionTargets(playerID, actionsToExecute, actionType, newAction);
		else
			verifyEntityActionTargets(playerID, actionsToExecute, actionType, newAction);

		reset();
	}

	void TBSActionsWidget::verifyPlayerActionTargets(int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
	{
		//Generate action targets + source
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
		actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());

		newAction.setActionTargets(actionTargets);
		newAction.setOwnerID(playerID);

		auto* player = state->getPlayer(playerID);

		if (player->canExecuteAction(actionTypeSelected))
		{
			if (state->getCurrentTick() - player->getActionInfo(actionTypeSelected).lastExecutedTick < actionType.getCooldown())
				return;

			if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
				actionsToExecute.emplace_back(newAction);
		}
	}

	void TBSActionsWidget::verifyEntityActionTargets(int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
	{
		//Generate action targets + source
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
		actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());

		newAction.setActionTargets(actionTargets);
		newAction.setOwnerID(playerID);

		//Generate all action for each entity
		for (auto& entityID : selectedEntities)
		{
			//Check if entityType can execute this
			const EntityType& entityType = state->getEntityConst(entityID)->getEntityType();
			const Entity* entity = state->getEntityConst(entityID);


			if (!entityType.canExecuteAction(actionTypeSelected))
				continue;
			// Check if this action can be executed		
			if (state->getCurrentTick() - entity->getActionInfo(actionTypeSelected).lastExecutedTick < actionType.getCooldown())
				continue;

			//The entity should be able to execute this action type
			newAction.getTargets()[0] = ActionTarget::createEntityActionTarget(entityID);

			if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
				actionsToExecute.emplace_back(newAction);
		}
	}

	bool TBSActionsWidget::checkSelectedEntitiesAvailable()
	{
		if (selectedEntities.empty())
			return false;

		bool areAvailable = true;
		for (auto& entityID : selectedEntities)
		{
			auto* entity = state->getEntityConst(entityID);
			if (!entity)
				areAvailable = false;
		}

		if (!areAvailable)
			selectedEntities.clear();

		return areAvailable;
	}

	void TBSActionsWidget::getActionType(int playerID)
	{
		std::unordered_set<int> actionTypes;


		if (!checkSelectedEntitiesAvailable())
			getPlayerPossibleActionTypes(playerID, actionTypes);
		else
			getEntityPossibleActionTypes(actionTypes);


		//Show buttons
		int elementNumber = 0;
		for (auto& actionType : actionTypes)
		{
			ImGui::PushID(elementNumber);
			if (ImGui::Button(state->getGameInfo()->getActionType(actionType).getName().c_str(), ImVec2(50, 50)))
			{
				actionTypeSelected = actionType;
			}
			if ((elementNumber++ % 4) < 3) ImGui::SameLine();
			ImGui::PopID();
		}
	}

	void TBSActionsWidget::getPlayerPossibleActionTypes(int playerID, std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");

		for (auto& attachedActions : state->getPlayer(playerID)->getAttachedActions())
		{
			actionTypes.insert(attachedActions.actionTypeID);
		}
	}

	void TBSActionsWidget::getEntityPossibleActionTypes(std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");

		for (auto& entity : selectedEntities)
		{
			int entityTypeID = state->getEntityConst(entity)->getEntityTypeID();

			for (const auto& actionID : state->getGameInfo()->getEntityType(entityTypeID).getActionIDs())
			{
				actionTypes.insert(actionID);
			}
		}
	}


	void TBSActionsWidget::assignPosition(Vector2f position)
	{
		auto positionTarget = SGA::ActionTarget::createPositionActionTarget(position);

		selectedTargets.emplace_back(positionTarget);
	}

	void TBSActionsWidget::assignEntity(int entity)
	{
		auto entityTarget = SGA::ActionTarget::createEntityActionTarget(entity);

		selectedTargets.emplace_back(entityTarget);
	}
}