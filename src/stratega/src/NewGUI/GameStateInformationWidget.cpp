#include <Stratega/NewGUI/GameStateInformationWidget.h>
#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip> 
#include <sstream>
namespace SGA
{
	GameStateInformationWidget::GameStateInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm):
		SGAWidget(widgetName, newWindow, newWorld, fm)
	{
	}

	void GameStateInformationWidget::update(const GameState& state)
	{
		this->state = &state;
	}
	
	void GameStateInformationWidget::render(SGARenderTarget& renderTarget)
	{
		ImGui::Begin("State info");
		std::string text = "Tick: " + std::to_string(state->getCurrentTick())+ "/"+ std::to_string(state->getTickLimit());
		ImGui::Text(text.c_str());
		text = "Current Player : " + std::to_string(state->getCurrentTBSPlayer());
		ImGui::Text(text.c_str());
		ImGui::End();
	}

}