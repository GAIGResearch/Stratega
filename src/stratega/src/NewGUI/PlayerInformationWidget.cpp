#include <Stratega/NewGUI/PlayerInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	PlayerInformationWidget::PlayerInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM):
		SGAWidget(widgetName, newWindow, newWorld, newFM)
	{
	}

	void PlayerInformationWidget::update(const GameState& newState)
	{
		this->state = &newState;
	}
	
	void PlayerInformationWidget::render(SGARenderTarget& /*renderTarget*/)
	{


		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20, 50), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::Begin("Player parameters");
		ImGui::BeginChild("Scrolling");
		ImGui::BeginGroup();

		if (world.getFOWSettings().selectedPlayerID != -2)
		{
			const auto* player = state->getPlayer(world.getFOWSettings().selectedPlayerID);
			for (const auto& parameter : state->getGameInfo()->getPlayerParameterTypes())
			{
				//Double to string with 2 precision
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << player->getRawParameterAt(parameter.second.getIndex());
				std::string valueParameter = stream.str();

				std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
				ImGui::BulletText("%s", parameterInfo.c_str());
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::End();
	}

}