#include <Stratega/GUI/Widgets/ActionsController.h>
#include <Stratega/ForwardModel/TargetType.h>
#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Action.h>

#include <imgui.h>

namespace SGA::Widgets
{
	bool ActionsSettings::hasActionsTargetAvailable(const ActionType& actionType) const
	{
		return selectedTargets.size() < actionType.actionTargets.size();
	}

	void getActionTarget(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
	{
		//Draw target
		auto& targetType = actionType.actionTargets[settings.selectedTargets.size()].first;
		switch (targetType.type)
		{
			case TargetType::EntityType:
			{
				getEntityType(state, settings, playerID, actionType);
				break;
			}
			case TargetType::Technology:
			{
				getTechnologyType(state, settings, playerID, actionType, actionsToExecute);
				break;
			}
			case TargetType::Position:
			{
				getPositionReference(settings);
				break;
			}
			case TargetType::Entity:
			{
				getEntityReference(settings);
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

	std::vector<Action> getWidgetResult(GameState& state, ActionsSettings& settings, int playerID)
	{
		std::vector<Action> actionsToExecute;
		//Check if the we have action type selected
		if (settings.hasActionTypeSelected())
		{
			//Selected actiontype
			const ActionType& actionType = state.gameInfo->getActionType(settings.actionTypeSelected);

			//Check if we have selected each action target
			if (settings.hasActionsTargetAvailable(actionType))
			{
				getActionTarget(state, settings, playerID, actionType,actionsToExecute);
			}
			else
			{
				verifyActionTargets(state, settings, playerID, actionsToExecute);
			}
		}
		else
		{
			getActionType(state, settings, playerID);
		}

		return actionsToExecute;
	}

	void getTechnologyType(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType, std::vector<Action>& actionsToExecute)
	{
		if(settings.hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& possibleTechnology : actionType.actionTargets[settings.selectedTargets.size()].first.technologyTypes)
			{
				//If player is researching abort action
				for (auto& entityID : settings.selectedEntities)
				{
					for (const auto& action : state.getEntity(entityID)->getContinuousActions())
					{
						if (action.getActionTypeID() == settings.actionTypeSelected)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
							if (ImGui::Button("Abort", ImVec2(50, 50)))
							{
								actionsToExecute.emplace_back(Action::createAbortAction(playerID, state.getEntity(entityID)->id, action.continuousActionID));
							}
							ImGui::PopStyleColor(3);

							goto  FINISH1;
						}
					}
				}

				//Check if the player can research the possible technologies
				if (!state.canResearch(playerID, possibleTechnology))
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.gameInfo->technologyTreeCollection->getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
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
		FINISH1:
			return;
		}
		else
		{
			int elementNumber = 0;
			for (auto& possibleTechnology : actionType.actionTargets[settings.selectedTargets.size()].first.technologyTypes)
			{
				//If player is researching abort action
				for (auto& action : state.getPlayer(playerID)->continuousAction)
				{
					if (action.getActionTypeID() == settings.actionTypeSelected)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0., 0.7f, 0.7f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
						if (ImGui::Button("Abort", ImVec2(50, 50)))
						{
							actionsToExecute.emplace_back(Action::createAbortAction(playerID, action.continuousActionID));
						}
						ImGui::PopStyleColor(3);

						goto  FINISH2;
					}
				}

				//Check if the player can research the possible technologies
				if (!state.canResearch(playerID, possibleTechnology))
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.gameInfo->technologyTreeCollection->getTechnology(possibleTechnology).name.c_str(), ImVec2(50, 50)))
				{
					//Check if the player fullfill the technology
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
					actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));

					//Check if player fullfil the conditions to spawn the entity
					for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
					{
						if (!condition->isFullfilled(state, actionTargets))
						{
							canExecute = false;
							break;
						}
					}

					if (canExecute)
						settings.selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(possibleTechnology));
				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();


			}
		FINISH2:
			return;
		}
	}

	void getEntityType(GameState& state, ActionsSettings& settings, int playerID, const ActionType& actionType)
	{
		if(settings.hasEntitiesSelected())
		{
			int elementNumber = 0;
			for (auto& possibleActionType : actionType.actionTargets[settings.selectedTargets.size()].first.groupEntityTypes)
			{
				bool canSpawn = true;

				//Check if any selectedEntity can spawn the targets
				for (auto& entity : settings.selectedEntities)
				{
					auto& entityType = state.getEntity(entity)->getEntityType();
					if (entityType.spawnableEntityTypes.find(possibleActionType) == entityType.spawnableEntityTypes.end())
					{
						canSpawn = false;
					}
				}

				if (!canSpawn)
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.gameInfo->getEntityType(possibleActionType).name.c_str(), ImVec2(50, 50)))
				{
					bool canExecute = true;

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
							if (!condition->isFullfilled(state, actionTargets))
							{
								canExecute = false;
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
		}
		else
		{
			int elementNumber = 0;
			for (auto& possibleActionType : actionType.actionTargets[settings.selectedTargets.size()].first.groupEntityTypes)
			{
				bool canSpawn = true;

				if (state.gameInfo->playerSpawnableTypes->find(possibleActionType) == state.gameInfo->playerSpawnableTypes->end())
				{
					canSpawn = false;
				}

				if (!canSpawn)
					continue;

				ImGui::PushID(elementNumber);
				if (ImGui::Button(state.gameInfo->getEntityType(possibleActionType).name.c_str(), ImVec2(50, 50)))
				{
					bool canExecute = true;

					std::vector<ActionTarget> actionTargets;
					actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
					actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());
					actionTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

					//Check if player fullfil the conditions to spawn the entity
					for (auto& condition : actionType.actionTargets[settings.selectedTargets.size()].second)
					{
						if (!condition->isFullfilled(state, actionTargets))
						{
							canExecute = false;
							break;
						}
					}

					if (canExecute)
						settings.selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(possibleActionType));

				}
				if ((elementNumber++ % 4) < 3) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		
	}
	
	void getPositionReference (ActionsSettings& settings)
	{
		ImGui::Text("Choose position");
		//Need to receive a new position from the gui
		settings.waitingForPosition = true;
	}
	
	void getEntityReference(ActionsSettings& settings)
	{
		ImGui::Text("Choose entity");
		//Need to receive a new position from the gui
		settings.waitingForEntity = true;
	}
	

	void verifyActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute)
	{
		//Verify the selected targets are valid						
		const ActionType& actionType = state.gameInfo->getActionType(settings.actionTypeSelected);

		//Generate action with the current selected settings
		Action newAction(&actionType);

		if (actionType.isContinuous)
			newAction.actionTypeFlags = ActionFlag::ContinuousAction;
		
		if (settings.selectedEntities.empty())
			verifyPlayerActionTargets(state, settings, playerID, actionsToExecute, actionType, newAction);
		else		
			verifyEntityActionTargets(state, settings, playerID, actionsToExecute, actionType, newAction);
		
		settings.reset();		
	}
	
	void verifyPlayerActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
	{
		//Generate action targets + source
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createPlayerActionTarget(playerID));
		actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());

		newAction.targets = actionTargets;
		newAction.ownerID = playerID;
		
		auto* player = state.getPlayer(playerID);
		
		if (player->canExecuteAction(settings.actionTypeSelected))
		{			
			if (state.currentTick - player->getActionInfo(settings.actionTypeSelected).lastExecutedTick < actionType.cooldownTicks)
				return;			

			if (ActionTarget::isValid(state, actionType, newAction.targets))
				actionsToExecute.emplace_back(newAction);
		}
	}

	void verifyEntityActionTargets(GameState& state, ActionsSettings& settings, int playerID, std::vector<Action>& actionsToExecute, const ActionType& actionType, Action& newAction)
	{
		//Generate action targets + source
		std::vector<ActionTarget> actionTargets;
		actionTargets.emplace_back(ActionTarget::createEntityActionTarget(0));
		actionTargets.insert(actionTargets.end(), settings.selectedTargets.begin(), settings.selectedTargets.end());

		newAction.targets = actionTargets;
		newAction.ownerID = playerID;

		//Generate all action for each entity
		for (auto& entityID : settings.selectedEntities)
		{
			//Check if entityType can execute this
			const EntityType& entityType = state.getEntity(entityID)->getEntityType();
			const Entity* entity = state.getEntity(entityID);

			
			if (!entityType.canExecuteAction(settings.actionTypeSelected))
				continue;
			// Check if this action can be executed		
			if (state.currentTick - entity->getActionInfo(settings.actionTypeSelected).lastExecutedTick < actionType.cooldownTicks)
				continue;			

			//The entity should be able to execute this action type
			newAction.targets[0] = ActionTarget::createEntityActionTarget(entityID);

			if (ActionTarget::isValid(state, actionType, newAction.targets))
				actionsToExecute.emplace_back(newAction);
		}
	}

	
	void getActionType(GameState& state, ActionsSettings& settings, int playerID)
	{
		std::unordered_set<int> actionTypes;
		
		if (settings.selectedEntities.empty())
			getPlayerPossibleActionTypes(state, settings, playerID, actionTypes);
		else
			getEntityPossibleActionTypes(state, settings, playerID, actionTypes);


		//Show buttons
		int elementNumber = 0;
		for (auto& actionType : actionTypes)
		{
			ImGui::PushID(elementNumber);
			if (ImGui::Button(state.gameInfo->getActionType(actionType).name.c_str(), ImVec2(50, 50)))
			{
				settings.actionTypeSelected = actionType;
			}
			if ((elementNumber++ % 4) < 3) ImGui::SameLine();
			ImGui::PopID();
		}
	}
	
	void getPlayerPossibleActionTypes(GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");

		for (auto& attachedActions : state.getPlayer(playerID)->attachedActions)
		{
			actionTypes.insert(attachedActions.actionTypeID);
		}
	}
	
	void getEntityPossibleActionTypes(GameState& state, ActionsSettings& settings, int playerID, std::unordered_set<int>& actionTypes)
	{
		//Display actionTypes
		ImGui::Text("Select action type");		

		for (auto& entity : settings.selectedEntities)
		{
			int entityTypeID = state.getEntity(entity)->getEntityTypeID();

			for (auto& actionID : state.gameInfo->getEntityType(entityTypeID).actionIds)
			{
				actionTypes.insert(actionID);
			}
		}
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
