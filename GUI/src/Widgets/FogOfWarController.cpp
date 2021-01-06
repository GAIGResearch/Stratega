#include <Widgets/FogOfWarController.h>
#include <imgui.h>

namespace SGA::Widgets
{
	bool fowController(const GameState& state, FogOfWarSettings& fowSettings)
	{
		static std::unordered_map<FogRenderType, std::string> nameLookup =
		{
			{FogRenderType::Nothing, "Nothing"},
			{FogRenderType::Fog, "Fog"},
			{FogRenderType::Tiles, "Tiles"},
		};

		// Select FogType
		ImGui::Checkbox("Is Active", &fowSettings.renderFogOfWar);
		if (ImGui::BeginCombo("FogType", nameLookup[fowSettings.renderType].c_str()))
		{
			for (const auto& typeNamePair : nameLookup)
			{
				auto isSelected = fowSettings.renderType == typeNamePair.first;
				if (ImGui::Selectable(typeNamePair.second.c_str(), isSelected))
				{
					fowSettings.renderType = typeNamePair.first;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		bool selectionChanged = false;
		if (ImGui::BeginCombo("View", ("Player " + std::to_string(fowSettings.selectedPlayerID)).c_str()))
		{
			for (const auto& player : state.players)
			{
				auto isSelected = fowSettings.selectedPlayerID == player.id;
				if (ImGui::Selectable(("Player " + std::to_string(player.id)).c_str(), isSelected))
				{
					fowSettings.selectedPlayerID = player.id;
					selectionChanged = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		return selectionChanged;
	}
}