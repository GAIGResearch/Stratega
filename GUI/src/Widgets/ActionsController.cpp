#include <Widgets/ActionsController.h>
#include <imgui.h>

#include <ForwardModel/TargetType.h>
#include <ForwardModel/ActionType.h>
#include <Representation/GameState.h>

namespace SGA::Widgets
{
	std::vector<Action> getWidgetResult(GameState& state, ActionsSettings& settings)
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
					for (auto& actionType1 : actionType.actionTargets[settings.selectedTargets.size()].first.groupEntityTypes)
					{
						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.getEntityType(actionType1).name.c_str(), ImVec2(50, 50)))
						{
							settings.selectedTargets.emplace_back(ActionTarget::createEntityTypeActionTarget(actionType1));

						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();
					}
					break;
				}
				case TargetType::Technology:
				{
					int elementNumber = 0;
					for (auto& actionType1 : actionType.actionTargets[settings.selectedTargets.size()].first.technologyTypes)
					{
						ImGui::PushID(elementNumber);
						if (ImGui::Button(state.technologyTreeCollection->getTechnology(actionType1).name.c_str(), ImVec2(50, 50)))
						{
							settings.selectedTargets.emplace_back(ActionTarget::createTechnologyEntityActionTarget(actionType1));

						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();
					}
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
					bool isSelected = true;

					for (int i = 0; i < settings.selectedTargets.size(); ++i)
					{
						if (settings.selectedTargets[i] != selectedAction.targets[i + 1])
						{
							isSelected = false;
						}
					}

					//Draw the action
					if (isSelected)
					{
						actionsToExecute.emplace_back(selectedAction);
						/*playAction(selectedAction);
						ImGui::PushID(elementNumber);
						if (ImGui::Button("Action", ImVec2(50, 50)))
						{

						}
						if ((elementNumber++ % 4) < 3) ImGui::SameLine();
						ImGui::PopID();*/
					}
				}
				
				//playAction(actionsToExecute);
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
			ImGui::Text("Actions");
			int numberOfEntities = settings.selectedUnits.size();

			std::unordered_set<int> actionTypes;

			for (auto& entity : settings.selectedUnits)
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

	void assignPosition(const GameState& state, ActionsSettings& settings, Vector2f position)
	{
		auto positionTarget = SGA::ActionTarget::createPositionActionTarget(position);

		settings.selectedTargets.emplace_back(positionTarget);
	}

}
