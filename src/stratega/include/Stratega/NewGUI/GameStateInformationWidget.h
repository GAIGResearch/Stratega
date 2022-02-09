#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct GameStateInformationWidget : public SGAWidget
	{
		GameStateInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM);

		void update(const GameState& newState) override;
		void render(SGARenderTarget& renderTarget) override;

	private:
		const GameState* state;
	};
}