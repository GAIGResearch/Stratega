#include <Stratega/NewGUI/ActionsWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	ActionsWidget::ActionsWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM, ActionAssignment& newActionAssignment, ActionAssignment& newFutureActionsToPlay, std::unordered_set<int>& newSelectedEntities, int& newPlayerID) :
		SGAWidget(widgetName, newWindow, newWorld, newFM),
		state(nullptr),
		selectedEntities(newSelectedEntities),
		actionAssignment(newActionAssignment),
		futureActionsToPlay(newFutureActionsToPlay),
		currentPlayerID(newPlayerID)
	{
	}

	void ActionsWidget::update(const GameState& newState)
	{
		this->state = &newState;
		if (currentPlayerID != -2)
			actionsHumanPlayer = fm->generateActions(newState, currentPlayerID);
	}

	void ActionsWidget::render(SGARenderTarget& renderTarget)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags += ImGuiWindowFlags_NoTitleBar;
		//window_flags += ImGuiWindowFlags_NoScrollbar;
		//window_flags += ImGuiWindowFlags_MenuBar;
		//window_flags+= ImGuiWindowFlags_NoMove;
		//window_flags += ImGuiWindowFlags_NoResize;
		//window_flags += ImGuiWindowFlags_NoCollapse;
		window_flags += ImGuiWindowFlags_NoNav;
		//window_flags+= ImGuiWindowFlags_NoBackground;
		window_flags += ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
		// We specify a default position/size in case there's no data in the .ini file.
		// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
		//ImGui::SetNextWindowPos(ImVec2((static_cast<float>(window.getSize().x) / 2.f), (static_cast<float>(window.getSize().y) / 1.1f)), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		//ImGui::SetNextWindowPos(ImVec2((static_cast<float>(window.getSize().x) / 2.f), (static_cast<float>(window.getSize().y) / 1.1f)), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x), (static_cast<float>(window.getSize().y))), ImGuiCond_FirstUseEver, ImVec2(1.0f, 1.0f));


		ImGui::Begin("Bottom Bar", NULL, window_flags);

		ImGui::Separator();

		ImGui::Text("Actions");

		//Ask widget to get
		if (currentPlayerID != -1)
		{
			auto actionsToExecute = getWidgetResult(currentPlayerID);
			if (state->getGameType() == GameType::TBS)
			{
				if (!actionsToExecute.empty())
				{
					actionAssignment.clear();
					actionAssignment.assignActionOrReplace(actionsToExecute.front());
				}
			}
			else
			{
				for (auto& action : actionsToExecute)
				{
					if (action.getActionTypeID() == -1 || action.isPlayerAction())
					{
						//If continuous or Player action we assign it directly
						actionAssignment.assignActionOrReplace(action);
					}
					else
					{
						//If is Entity action we save it for future calls
						futureActionsToPlay.assignActionOrReplace(action);
					}
				}
			}

		}

		ImGui::Separator();
		ImGui::End();


		ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 150), ImGuiCond_FirstUseEver);

		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
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
					case ActionTarget::Gamestate:
						break;
					case ActionTarget::TileReference:
						actionInfo += /*targetType.getTileType(*state).getName() + */" x:" + std::to_string(static_cast<int>(targetType.getPosition(*state).x)) + ",y:" + std::to_string(static_cast<int>(targetType.getPosition(*state).y));
						break;
					case ActionTarget::Object:
						break;
					case ActionTarget::SlotObject:
						break;
					}
				}

			}

			index++;

			if (ImGui::Button(actionInfo.c_str()))
			{
				actionAssignment.clear();
				actionAssignment.assignActionOrReplace(action);
				break;
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();

		float circleShapeSize = static_cast<float>(renderTarget.getResourceManager().getTileSpriteSize().y) / 10.0f;
		//Draw possible actions
		std::vector<sf::CircleShape> actionsShapes;
		if (waitingForPosition || waitingForTile)
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
					if (actionTarget.getType() == ActionTarget::Position
						|| actionTarget.getType() == ActionTarget::TileReference)
					{
						auto position = actionTarget.getPosition(*state);

						sf::CircleShape possibleActionPositionShape(circleShapeSize);
						possibleActionPositionShape.setFillColor(sf::Color::White);
						possibleActionPositionShape.setOrigin(circleShapeSize * 0.5f, circleShapeSize * 0.5f);
						sf::Vector2f temp = world.toSFML(position);
						sf::Vector2f offset;
						if (renderTarget.getRenderConfig().isIsometricGrid)
							offset = sf::Vector2f{ 0, static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.y / 2) };
						else
							offset = sf::Vector2f{ static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.x / 2), static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.y / 2) };
						possibleActionPositionShape.setPosition(temp + offset);

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

						sf::CircleShape possibleActionPositionShape(circleShapeSize);
						possibleActionPositionShape.setFillColor(sf::Color::White);
						possibleActionPositionShape.setOrigin(circleShapeSize * 0.5f, circleShapeSize * 0.5f);
						sf::Vector2f temp = world.toSFML(position);

						sf::Vector2f offset;
						if (renderTarget.getRenderConfig().isIsometricGrid)
							offset = sf::Vector2f{ 0, static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.y / 2) };
						else
							offset = sf::Vector2f{ static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.x / 2), static_cast<float>(renderTarget.getRenderConfig().tileSpriteSize.y / 2) };
						possibleActionPositionShape.setPosition(temp + offset);

						actionsShapes.emplace_back(possibleActionPositionShape);
					}
				}

				for (const auto& shape : actionsShapes)
				{
					window.draw(shape);
				}
			}
		}


		// Handle dragging
		if (dragging)
		{
			auto currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto diff = oldMousePosition - currentPos;
			auto yLeft = std::min(currentPos.y, oldMousePosition.y);
			auto xLeft = std::min(currentPos.x, oldMousePosition.x);
			sf::RectangleShape selectedArea({ std::abs(diff.x), std::abs(diff.y) });
			selectedArea.setFillColor(sf::Color::Transparent);
			selectedArea.setOutlineColor(sf::Color::White);
			selectedArea.setOutlineThickness(10);
			selectedArea.setPosition(xLeft, yLeft);
			window.draw(selectedArea);
		}
	}

	void ActionsWidget::mouseButtonReleased(const sf::Event& event)
	{
		if (event.mouseButton.button == sf::Mouse::Left && state->getGameType() == GameType::RTS)
		{
			dragging = false;

			sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto worldPos = world.toStratega(pos);
			auto movementDir = pos - oldMousePosition;
			auto movementDistance = std::sqrt(movementDir.x * movementDir.x + movementDir.y * movementDir.y);

			if (movementDistance <= 10.0)
			{
				// The user clicked somewhere
				if (!selectedEntities.empty())
				{
					const auto* unit = state->getEntityAroundConst(SGA::Vector2f(worldPos.x, worldPos.y), 0.5);

					if (unit)
					{
						//we click on someone
						if (waitingForEntity)
						{
							assignEntity(unit->getID());
						}
						else
						{
							selectedEntities.clear();
						}

						possibleSelectedEntities.clear();
						waitingToSelectPossibleEntitie = false;
					}
					else
					{
						if (waitingForPosition)
						{
							auto gridPos = world.toStratega(pos);
							assignPosition({ gridPos.x,gridPos.y });
						}
						else if (waitingForTile)
						{
							auto gridPos = world.toStratega(pos);
							assignTile({ gridPos.x,gridPos.y });
						}
						else
						{
							selectedEntities.clear();
						}
					}
				}
				else
				{
					if (waitingForPosition)
					{
						auto gridPos = world.toStratega(pos);
						assignPosition({ static_cast<float>(gridPos.x),static_cast<float>(gridPos.y) });
					}
					else if (waitingForTile)
					{
						auto gridPos = world.toStratega(pos);
						assignTile({ static_cast<float>(gridPos.x),static_cast<float>(gridPos.y) });
					}
					else
					{
						selectedEntities.clear();
						reset();
					}
				}
			}
			else
			{
				// The user selected an area
				auto prevMouseWorldPos = oldMousePosition;
				auto xLeft = std::min(prevMouseWorldPos.x, pos.x);
				auto xRight = std::max(prevMouseWorldPos.x, pos.x);
				auto yLeft = std::min(prevMouseWorldPos.y, pos.y);
				auto yRight = std::max(prevMouseWorldPos.y, pos.y);

				for (const auto& unit : state->getEntities())
				{
					sf::Vector2f screenPos = world.toSFML(unit.getPosition());
					if (screenPos.x > xLeft && screenPos.x < xRight && screenPos.y > yLeft && screenPos.y < yRight)
					{
						if (unit.getOwnerID() == world.getFOWSettings().selectedPlayerID)
							selectedEntities.emplace(unit.getID());
					}
				}
			}
		}
	}

	void ActionsWidget::mouseButtonPressed(const sf::Event& event)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			Vector2i pos = world.toStrategaRounded(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
			//If selected unit we check if there is action in tile
			if (waitingForPosition)
			{
				assignPosition({ static_cast<float>(pos.x),static_cast<float>(pos.y) });
			}
			else if (waitingForTile)
			{
				assignTile({ static_cast<float>(pos.x),static_cast<float>(pos.y) });
			}
			else
			{
				if (!waitingForEntity)
				{
					selectedEntities.clear();
					actionTypeSelected = -1;
				}
			}

			if (state->getGameType() == GameType::TBS)
			{
				auto tileEntities = state->getEntitiesAtConst(Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)));
				if (tileEntities.size() == 1)
				{
					auto* selectedEntity = tileEntities[0];
					//if (selectedEntity /*&& playerID==0 *//*&& ((fowSettings.renderFogOfWar && (pointOfViewPlayerID == fowSettings.selectedPlayerID)) || !fowSettings.renderFogOfWar)*/)
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

					possibleSelectedEntities.clear();
					waitingToSelectPossibleEntitie = false;
				}
				else if (tileEntities.size() > 1)
				{
					auto* selectedEntity = tileEntities[0];

					waitingForEntity = false;
					waitingToSelectPossibleEntitie = true;
					for (auto possibleEntity : tileEntities)
					{
						possibleSelectedEntities.emplace(possibleEntity->getID());
					}
					//if (waitingForEntity)
					//{
					//	//assignEntity(selectedEntity->getID());
					//	waitingForEntity = false;
					//	waitingToSelectPossibleEntitie = true;
					//}
					//else
					//{
					//	//Pick up entity
					//	//if (selectedEntity->getOwnerID() == playerID)
					//	//selectedEntities.emplace(selectedEntity->getID());

					//}
				}
				//else
				//{
				//	//Restart selected actions of unit and selected unit
				//	dragging = true;
				//	oldMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				//}
			}
			else
			{
				oldMousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				dragging = true;
			}

		}
		if (event.mouseButton.button == sf::Mouse::Right && state->getGameType() == GameType::TBS)
		{
			//selectedAction = Action::createEndAction(pointOfViewPlayerID);
			actionAssignment.clear();
			actionAssignment = actionAssignment.createEndActionAssignment(state->getCurrentTBSPlayer());
		}
	}

	bool ActionsWidget::hasActionsTargetAvailable(const SGA::ActionType& actionType) const
	{
		return selectedTargets.size() < actionType.getTargets().size();
	}

	void ActionsWidget::getActionTarget(int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
	{
		//Draw target

		int elementNumber = 0;
		for (auto entityID : possibleSelectedEntities)
		{
			ImGui::PushID(elementNumber);
			if (ImGui::Button(state->getEntityConst(entityID)->getEntityType().getName().c_str(), ImVec2(50, 50)))
			{
				selectedEntities.emplace(entityID);
				waitingToSelectPossibleEntitie = false;
			}
			if ((elementNumber++ % 4) < 3) ImGui::SameLine();
			ImGui::PopID();
		}

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
		case TargetType::Tile:
		{
			getTileReference();
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
		case TargetType::Object:
		{
			getObject(targetType);
			break;
		}
		case TargetType::SlotObject:
		{
			getSlotObject(targetType);
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


	std::vector<Action> ActionsWidget::getWidgetResult(int playerID)
	{
		std::vector<Action> actionsToExecute;
		//Check if the we have action type selected
		if (playerID == -2)
			return actionsToExecute;




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
		else if (waitingToSelectPossibleEntitie)
		{
			int elementNumber = 0;
			for (auto entityID : possibleSelectedEntities)
			{
				ImGui::PushID(elementNumber);
				if (ImGui::Button(state->getEntityConst(entityID)->getEntityType().getName().c_str(), ImVec2(50, 50)))
				{
					selectedEntities.emplace(entityID);
					waitingToSelectPossibleEntitie = false;
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}
			return actionsToExecute;
		}
		else
		{
			getActionType(playerID);
		}

		return actionsToExecute;
	}


	void ActionsWidget::getTechnologyType(int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
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
				if (ImGui::Button(state->getGameInfo()->getTechnologyTreeCollection().getTechnology(possibleTechnology).name.c_str(), ImVec2(0, 50)))
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
				if (ImGui::Button(state->getGameInfo()->getTechnologyTreeCollection().getTechnology(possibleTechnology).name.c_str(), ImVec2(0, 50)))
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

	void ActionsWidget::getObject(const TargetType& /*actionType*/)
	{
		if (hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& entityID : selectedEntities)
			{
				const auto& entity = state->getEntityConst(entityID);
				for (auto& object : entity->getInventory())
				{
					ImGui::PushID(elementNumber);
					if (ImGui::Button(object.getEntityType().getName().c_str(), ImVec2(0, 50)))
					{
						selectedTargets.emplace_back(ActionTarget::createObjectActionTarget(object.getID()));
					}
					if ((elementNumber++ % 4) < 3) ImGui::SameLine();
					ImGui::PopID();
				}
			}
		}
	}

	void ActionsWidget::getSlotObject( const TargetType& /*actionType*/)
	{
		if (hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& entityID : selectedEntities)
			{
				const auto& entity = state->getEntityConst(entityID);
				for (const auto& object : entity->getSlots())
				{
					ImGui::PushID(elementNumber);
					if (ImGui::Button(object.first.getEntityType().getName().c_str(), ImVec2(0, 50)))
					{
						selectedTargets.emplace_back(ActionTarget::createSlotObjectActionTarget(object.first.getID()));
					}
					if ((elementNumber++ % 4) < 3) ImGui::SameLine();
					ImGui::PopID();
				}
			}
		}
	}

	void ActionsWidget::getEntityType(int playerID, const ActionType& actionType)
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
				if (ImGui::Button(state->getGameInfo()->getEntityType(possibleActionType).getName().c_str(), ImVec2(0, 50)))
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
				if (ImGui::Button(state->getGameInfo()->getEntityType(possibleActionType).getName().c_str(), ImVec2(0, 50)))
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

	void ActionsWidget::getPositionReference()
	{
		ImGui::Text("Choose position");
		//Need to receive a new position from the gui
		waitingForPosition = true;
	}

	void ActionsWidget::getTileReference()
	{
		ImGui::Text("Choose tile");
		//Need to receive a new position from the gui
		waitingForTile = true;
	}

	void ActionsWidget::getEntityReference()
	{
		ImGui::Text("Choose entity");

		//Need to receive a new position from the gui
		waitingForEntity = true;
	}


	void ActionsWidget::verifyActionTargets(int playerID, std::vector<Action>& actionsToExecute)
	{
		//Verify the selected targets are valid						
		const ActionType& actionType = state->getGameInfo()->getActionType(actionTypeSelected);

		//Generate action with the current selected settings
		Action newAction(&actionType);

		if (actionType.isContinuous())
			newAction.setActionFlag(ActionFlag::ContinuousAction);
		if (actionType.isEndTick())
			newAction.setActionFlag(ActionFlag::EndTickAction);

		if (selectedEntities.empty())
			verifyPlayerActionTargets(playerID, actionsToExecute, actionType, newAction);
		else
			verifyEntityActionTargets(playerID, actionsToExecute, actionType, newAction);

		reset();
	}

	void ActionsWidget::verifyPlayerActionTargets(int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
	{
		//Generate action targets + source
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
		actionTargets.insert(actionTargets.end(), selectedTargets.begin(), selectedTargets.end());

		newAction.setActionTargets(actionTargets);
		newAction.setOwnerID(playerID);

		auto* player = state->getPlayer(playerID);

		if (state->getActionQueuesConst().hasActionInPlayerQueue(playerID) &&
			state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceType == ActionQueuePack::ActionSourceType::Player &&
			state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceID == playerID)
		{
			if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
				actionsToExecute.emplace_back(newAction);
		}	
		else if (player->canExecuteAction(actionTypeSelected))
		{
			if (state->getCurrentTick() - player->getActionInfo(actionTypeSelected).lastExecutedTick < actionType.getCooldown())
				return;

			if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
				actionsToExecute.emplace_back(newAction);
		}
	}

	void ActionsWidget::verifyEntityActionTargets(int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
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

			if (state->getActionQueuesConst().hasActionInPlayerQueue(playerID) &&
				state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceType == ActionQueuePack::ActionSourceType::Entity &&
				state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceID == entityID)
			{
				newAction.getTargets()[0] = ActionTarget::createEntityActionTarget(entityID);

				if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
				{
					actionsToExecute.emplace_back(newAction);
				}
			}
			else if (entityType.canExecuteAction(actionTypeSelected))
			{
				// Check if this action can be executed		
				if (state->getCurrentTick() - entity->getActionInfo(actionTypeSelected).lastExecutedTick < actionType.getCooldown())
					continue;

				//The entity should be able to execute this action type
				newAction.getTargets()[0] = ActionTarget::createEntityActionTarget(entityID);

				if (ActionTarget::isValidWithTargets(*state, actionType, newAction.getTargets()))
					actionsToExecute.emplace_back(newAction);

			}
			else
				continue;

		}
	}

	bool ActionsWidget::checkSelectedEntitiesAvailable()
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

	void ActionsWidget::getActionType(int playerID)
	{
		std::unordered_set<int> actionTypes;


		if (!checkSelectedEntitiesAvailable())
			getPlayerPossibleActionTypes(playerID, actionTypes);
		else
			getEntityPossibleActionTypes(playerID, actionTypes);


		//Show buttons
		int elementNumber = 0;
		for (auto& actionType : actionTypes)
		{
			ImGui::PushID(elementNumber);
			if (ImGui::Button(state->getGameInfo()->getActionType(actionType).getName().c_str(), ImVec2(0, 50)))
			{
				actionTypeSelected = actionType;
			}
			if ((elementNumber++ % 4) < 3) ImGui::SameLine();
			ImGui::PopID();
		}
	}

	void ActionsWidget::getPlayerPossibleActionTypes(int playerID, std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");
		//Add action queue
		if (state->getActionQueuesConst().hasActionInPlayerQueue(playerID))
		{
			if (state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceType == ActionQueuePack::ActionSourceType::Player)
				actionTypes.insert(state->getActionQueuesConst().getActionFromPlayerQueue(playerID).actionTypeID);
		}
		else
		{
			for (auto& attachedActions : state->getPlayer(playerID)->getAttachedActions())
			{
				actionTypes.insert(attachedActions.actionTypeID);
			}
		}
	}

	void ActionsWidget::getEntityPossibleActionTypes(int playerID, std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");

		if (state->getActionQueuesConst().hasActionInPlayerQueue(playerID))
		{
			if (state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceType == ActionQueuePack::ActionSourceType::Entity)
			{
				int sourceID = state->getActionQueuesConst().getActionFromPlayerQueue(playerID).sourceID;
				for (int entity : selectedEntities)
				{
					if (entity == sourceID)
						actionTypes.insert(state->getActionQueuesConst().getActionFromPlayerQueue(playerID).actionTypeID);
				}
			}
		}
		else
		{
			for (auto& entity : selectedEntities)
			{
				int entityTypeID = state->getEntityConst(entity)->getEntityTypeID();

				for (const auto& actionID : state->getGameInfo()->getEntityType(entityTypeID).getActionIDs())
				{
					actionTypes.insert(actionID);
				}
			}
		}
	}


	void ActionsWidget::assignPosition(Vector2f position)
	{
		auto positionTarget = SGA::ActionTarget::createPositionActionTarget(position);

		selectedTargets.emplace_back(positionTarget);
	}

	void ActionsWidget::assignTile(Vector2f position)
	{
		auto positionTarget = SGA::ActionTarget::createTileActionTarget(position);

		selectedTargets.emplace_back(positionTarget);
	}

	void ActionsWidget::assignEntity(int entity)
	{
		auto entityTarget = SGA::ActionTarget::createEntityActionTarget(entity);

		selectedTargets.emplace_back(entityTarget);
		possibleSelectedEntities.clear();
		waitingToSelectPossibleEntitie = false;
	}
}