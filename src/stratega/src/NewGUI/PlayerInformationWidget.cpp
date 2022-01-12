#include <Stratega/NewGUI/PlayerInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	PlayerInformationWidget::PlayerInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm):
		SGAWidget(widgetName, newWindow, newWorld, fm)
	{
	}

	void PlayerInformationWidget::update(const GameState& state)
	{
		this->state = &state;
	}
	
	void PlayerInformationWidget::render(SGARenderTarget& renderTarget)
	{
		ImGui::SetNextWindowSize(ImVec2(100, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("PlayerParameters");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		if (true/*pointOfViewPlayerID != NO_PLAYER_ID*/)
		{
			const auto* player = state->getPlayer(state->getCurrentTBSPlayer()/*fowSettings.selectedPlayerID*/);
			for (const auto& parameter : state->getGameInfo()->getPlayerParameterTypes())
			{
				//Double to string with 2 precision
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << player->getParameter(parameter.second.getIndex());
				std::string valueParameter = stream.str();

				std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
				ImGui::BulletText(parameterInfo.c_str());
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

}