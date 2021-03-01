#include <Widgets/ActionsController.h>
#include <imgui.h>

#include <Stratega/ForwardModel/TargetType.h>
#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Action.h>

namespace SGA::Widgets
{
	std::vector<Action> getWidgetResult(GameState& state, ActionsSettings& settings, int playerID)
	{
		//Check if we have selected entities
		if (!settings.selectedEntities.empty())
			return getEntityActions(state, settings, playerID);
		else
			return getPlayerActions(state, settings, playerID);
	}

	std::vector<Action> getEntityActions(GameState& state, ActionsSettings& settings, int playerID)
	{
		std::vector<Action> actionsToExecute;
		//Check if the we have action type selected
		if (settings.actionTypeSelected != -1)
		{
			//Selected actiontype
			const ActionType& actionType = state.getActionType(settings.actionTypeSelected);

			//Check if we have selected each action target
			if (settings.selectedTargets.size() < actionType.actionTargets.size())
			{
				//Draw target
				auto& targetType = actionType.actionTargets[settings.selectedTargets.size()].first;
				switch (targetType.type)
				{
				case TargetType::EntityType:
				{
					int elementNumber = 0;
					for (auto& possibleActionType : actionType.actionTargets[settings.selectedTargets.size()].first.groupEntityTypes)
					{
						bool canSpawn = true;

						//Check if any selectedEntity can spawn the targets
						for (auto& entity : settings.selectedEntities)
						{
							auto& entityType = state.getEntityType(state.getEntity(entity)->typeID);
							if (entityType.spawnableEntityTypes.find(possibleActionType) == entityType.spawnableEntityTypes.end())
							{
								canSpawn = false;
							}
						}

						if (!canSpawn)
							continue;

						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.getEntityType(possibleActionType).name.c_str(), ImVec2(50, 50)))
						{
							bool canExecute = false;

							std::vector<ActionTarget> actionTargets;
							actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
							actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
							actionTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

							//Check if any of the selectedEntities fullfill the condition
							for (auto& entity : settings.selectedEntities)
							{
								actionTargets[0] = (ActionTarget::createEntityActionTarget(entity));

								//Check if we fullfil the conditions to spawn the entity
								for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
								{
									if (condition->isFullfilled(state, actionTargets))
									{
										canExecute = true;
										break;
									}

								}
							}
							if (canExecute)
								settings.selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();
					}
					break;
				}
				case TargetType::Technology:
				{

					int elementNumber = 0;
					for (auto& possibleTechnology : actionType.actionTargets[settings.selectedTargets.size()].first.technologyTypes)
					{
						//If player is researching abort action
						for (auto& entityID : settings.selectedEntities)
						{
							for (auto& action : state.getEntity(entityID)->continuousAction)
							{
								if (action.actionTypeID == settings.actionTypeSelected)
								{
									ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
									ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
									ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
									if (ImGui::Button("Abort", ImVec2(50, 50)))
									{
										actionsToExecute.emplace_back(Action::createAbortAction(playerID, state.getEntity(entityID)->id, action.continuousActionID));
									}
									ImGui::PopStyleColor(3);

									goto  FINISH;
									break;
								}
							}
						}

						//Check if the player can research the possible technologies
						if (!state.technologyTreeCollection->canResearch(playerID, possibleTechnology))
							continue;

						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.technologyTreeCollection->getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
						{
							//Check if the player fullfill the technology
							bool canExecute = true;

							std::vector<ActionTarget> actionTargets;
							actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
							actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
							actionTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
							//Check if any of the selectedEntities fullfill the condition
							for (auto& entity : settings.selectedEntities)
							{
								actionTargets[0] = (ActionTarget::createEntityActionTarget(entity));

								//Check if we fullfil the conditions to spawn the entity
								for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
								{
									if (!condition->isFullfilled(state, actionTargets))
									{
										canExecute = false;
										break;
									}

								}
							}
							if (canExecute)
								settings.selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();


					}
				FINISH:
					break;
				}
				case TargetType::Position:
				{
					ImGui::Text("Choose position");
					//Need to receive a new position from the gui
					settings.waitingForPosition = true;

					break;
				}
				case TargetType::Entity:
				{
					ImGui::Text("Choose entity");
					//Need to receive a new position from the gui
					settings.waitingForEntity = true;

					break;
				}
				}


				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));

				//Reset button
				if (ImGui::Button("Reset", ImVec2(50, 50)))
				{
					settings.reset();
				}

				ImGui::PopStyleColor(3);
			}
			else
			{				
				//Verify the selected targets are valid						
				const ActionType& actionType = state.getActionType(settings.actionTypeSelected);
				
				//Generate action with the current selected settings
				Action newAction;

				//Generate action targets + source
				std::vector<ActionTarget> actionTargets;
				actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
				actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());

				newAction.targets = actionTargets;
				newAction.actionTypeID = settings.actionTypeSelected;
				newAction.ownerID = playerID;
				
				//Generate all action for each entity
				for (auto& entityID : settings.selectedEntities)
				{
					//Check if entityType can execute this
					const EntityType& entityType = state.getEntityType(state.getEntity(entityID)->typeID);

					if (!entityType.canExecuteAction(settings.actionTypeSelected))
						continue;
					
					//The entity should be able to execute this action type
					newAction.targets[0] = ActionTarget::createEntityActionTarget(entityID);

					if(ActionTarget::isValid(state, actionType, newAction.targets))
					actionsToExecute.emplace_back(newAction);
				}
				
				settings.reset();
			}

		}
		else
		{
			//Display actionTypes
			ImGui::Text("Select action type");
			int numberOfEntities = settings.selectedEntities.size();

			std::unordered_set<int> actionTypes;

			for (auto& entity : settings.selectedEntities)
			{
				int entityTypeID = state.getEntity(entity)->typeID;

				for (auto& actionID : state.getEntityType(entityTypeID).actionIds)

				{
					actionTypes.insert(actionID);
				}
			}

			int elementNumber = 0;
			for (auto& actionType : actionTypes)
			{
				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.getActionType(actionType).name.c_str(), ImVec2(50, 50)))
				{
					settings.actionTypeSelected = actionType;
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}

		}

		return actionsToExecute;
	}

	std::vector<Action> getPlayerActions(GameState& state, ActionsSettings& settings, int playerID)
	{
		std::vector<Action> actionsToExecute;
		//Check if the we have action type selected
		if (settings.actionTypeSelected != -1)
		{
			//Selected actiontype
			const ActionType& actionType = state.getActionType(settings.actionTypeSelected);

			//Check if we have selected each action target
			if (settings.selectedTargets.size() < actionType.actionTargets.size())
			{
				//Draw target
				auto& targetType = actionType.actionTargets[settings.selectedTargets.size()].first;
				switch (targetType.type)
				{
				case TargetType::EntityType:
				{
					int elementNumber = 0;
					for (auto& possibleActionType : actionType.actionTargets[settings.selectedTargets.size()].first.groupEntityTypes)
					{
						bool canSpawn = true;

						if (state.playerSpawnableTypes->find(possibleActionType) == state.playerSpawnableTypes->end())
						{
							canSpawn = false;
						}

						if (!canSpawn)
							continue;

						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.getEntityType(possibleActionType).name.c_str(), ImVec2(50, 50)))
						{
							bool canExecute = false;

							std::vector<ActionTarget> actionTargets;
							actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
							actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
							actionTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));
							
							//Check if player fullfil the conditions to spawn the entity
							for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
							{
								if (condition->isFullfilled(state, actionTargets))
								{
									canExecute = true;
									break;
								}
							}
							
							if (canExecute)
								settings.selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();
					}
					break;
				}
				case TargetType::Technology:
				{

					int elementNumber = 0;
					for (auto& possibleTechnology : actionType.actionTargets[settings.selectedTargets.size()].first.technologyTypes)
					{
						//If player is researching abort action
						for (auto& entityID : settings.selectedEntities)
						{
							for (auto& action : state.getEntity(entityID)->continuousAction)
							{
								if (action.actionTypeID == settings.actionTypeSelected)
								{
									ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
									ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
									ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
									if (ImGui::Button("Abort", ImVec2(50, 50)))
									{
										actionsToExecute.emplace_back(Action::createAbortAction(playerID, state.getEntity(entityID)->id, action.continuousActionID));
										/*selectedActionType = -1;
										updatePossibleActions();*/
									}
									ImGui::PopStyleColor(3);

									goto  FINISH;
									break;
								}
							}
						}

						//Check if the player can research the possible technologies
						if (!state.technologyTreeCollection->canResearch(playerID, possibleTechnology))
							continue;

						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.technologyTreeCollection->getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
						{
							//Check if the player fullfill the technology
							bool canExecute = true;

							std::vector<ActionTarget> actionTargets;
							actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
							actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
							actionTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
							//Check if any of the selectedEntities fullfill the condition
							for (auto& entity : settings.selectedEntities)
							{
								actionTargets[0] = (ActionTarget::createEntityActionTarget(entity));

								//Check if we fullfil the conditions to spawn the entity
								for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
								{
									if (!condition->isFullfilled(state, actionTargets))
									{
										canExecute = false;
										break;
									}

								}
							}
							if (canExecute)
								settings.selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();


					}
				FINISH:
					break;
				}
				case TargetType::Position:
				{
					//Need to receive a new position from the gui
					settings.waitingForPosition = true;

					break;
				}
				}


				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));

				//Reset button
				if (ImGui::Button("Reset", ImVec2(50, 50)))
				{
					settings.reset();
				}

				ImGui::PopStyleColor(3);
			}
			else
			{
				//Draw each action with the current selected action targets
				int elementNumber = 0;

				for (auto& selectedAction : settings.actionHumanSelected)
				{
					if (selectedAction.targets.size() <= 1 || selectedAction.actionTypeID != settings.actionTypeSelected)
						continue;

					bool isSelected = false;

					//Check if we have selected the owner of this action
					//for (auto entityID : settings.selectedUnits)
					//{
					//	if (selectedAction.getSourceID() == entityID)
					//	{
					//		//It the same entity owner
					//		isSelected = true;
					//		break;
					//	}
					//}

					//Check if all the targets of the actions are the same as the selected targets
					for (int i = 0; i < settings.selectedTargets.size(); ++i)
					{
						//Check if the selectedAction has lower amount of targets
						if (selectedAction.targets.size() < settings.selectedTargets.size() + 1)
						{
							settings.reset();
							isSelected = false;
							break;
						}
						
						if (settings.selectedTargets[i] != selectedAction.targets[i + 1])
						{
							isSelected = false;
						}
					}

					//Draw the action
					if (isSelected)
					{
						actionsToExecute.emplace_back(selectedAction);
					}
				}


				//Reset Button Style
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));

				//Reset button
				if (ImGui::Button("Reset", ImVec2(50, 50)))
				{
					settings.reset();
				}

				ImGui::PopStyleColor(3);
			}

		}
		else
		{
			//Display actionTypes
			ImGui::Text("Select Action Type");
			int numberOfEntities = settings.selectedEntities.size();

			std::unordered_set<int> actionTypes;

			for (auto& attachedActions : state.getPlayer(playerID)->attachedActions)
			{
				actionTypes.insert(attachedActions.actionTypeID);
			}

			int elementNumber = 0;
			for (auto& actionType : actionTypes)
			{
				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.getActionType(actionType).name.c_str(), ImVec2(50, 50)))
				{
					settings.actionTypeSelected = actionType;
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}

		}

		return actionsToExecute;
	}


	void assignPosition(const GameState& state, ActionsSettings& settings, Vector2f position)
	{
		auto positionTarget = SGA::ActionTarget::createPositionActionTarget(position);

		settings.selectedTargets.emplace_back(positionTarget);
	}

	void assignEntity(const GameState& state, ActionsSettings& settings, int entity)
	{
		auto entityTarget = SGA::ActionTarget::createEntityActionTarget(entity);

		settings.selectedTargets.emplace_back(entityTarget);
	}

}
