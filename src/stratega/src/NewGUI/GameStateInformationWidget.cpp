#include <Stratega/NewGUI/GameStateInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	GameStateInformationWidget::GameStateInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM):
		SGAWidget(widgetName, newWindow, newWorld, newFM)
	{
	}

	void GameStateInformationWidget::update(const GameState& newState)
	{
		this->state = &newState;
	}
	
	void GameStateInformationWidget::render(SGARenderTarget& /*renderTarget*/)
	{
		ImGui::Begin("State info");
		std::string text = "Tick: " + std::to_string(state->getCurrentTick()) + "/"+ std::to_string(state->getTickLimit());
		ImGui::Text("%s", text.c_str());
		text = "Current Player : " + std::to_string(state->getCurrentTBSPlayer());
		ImGui::Text("%s", text.c_str());

		ImGui::Separator();

		ImGui::Text("Parameters: ");

		for (const auto& parameter : state->getGameInfo()->getStateParameterTypes())
		{
			//Double to string with 2 precision
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << state->getRawParameterAt(parameter.second.getIndex());
			std::string valueParameter = stream.str();

			std::string parameterInfo = parameter.second.getName() + ": " + valueParameter;
			ImGui::BulletText("%s", parameterInfo.c_str());
		}

		ImGui::End();
	}

}