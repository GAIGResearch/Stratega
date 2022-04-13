#include <Stratega/NewGUI/FOWControllerWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	FOWControllerWidget::FOWControllerWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm, FogOfWarSettings* newSettings):
		SGAWidget(widgetName, newWindow, newWorld, newFm),
		fowSettings(newSettings),
		currentState(nullptr)
	{
	}

	void FOWControllerWidget::update(const GameState& state)
	{
		currentState = &state;
	}
	
	void FOWControllerWidget::render(SGARenderTarget& /*renderTarget*/)
	{
		static std::unordered_map<FogRenderType, std::string> nameLookup =
		{
			{FogRenderType::Nothing, "Nothing"},
			{FogRenderType::Fog, "Fog"},
			{FogRenderType::Tiles, "Tiles"},
		};

		// Select FogType
		// Select player
		bool selectionChanged = false;
		if (renderFogOfWarBefore != fowSettings->renderFogOfWar)
		{
			renderFogOfWarBefore = fowSettings->renderFogOfWar;
			selectionChanged = true;
		}

		ImGui::SetNextWindowPos(ImVec2(20, 130), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::Begin("Fog of war controller");
		ImGui::Checkbox("Is Active", &fowSettings->renderFogOfWar);
		if (ImGui::BeginCombo("FogType", nameLookup[fowSettings->renderType].c_str()))
		{
			for (const auto& typeNamePair : nameLookup)
			{
				auto isSelected = fowSettings->renderType == typeNamePair.first;
				if (ImGui::Selectable(typeNamePair.second.c_str(), isSelected))
				{
					fowSettings->renderType = typeNamePair.first;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		
		if (ImGui::BeginCombo("View", ("Player " + std::to_string(fowSettings->selectedPlayerID)).c_str()))
		{
			for (const auto& player : currentState->getPlayers())
			{
				auto isSelected = fowSettings->selectedPlayerID == player.getID();
				if (ImGui::Selectable(("Player " + std::to_string(player.getID())).c_str(), isSelected))
				{
					fowSettings->selectedPlayerID = player.getID();
					selectionChanged = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		this->fowSettings->settingsChanged = selectionChanged;
		ImGui::End();
	}
}