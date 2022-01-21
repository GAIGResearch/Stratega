#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct MouseInformationWidget : public SGAWidget
	{
		MouseInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

	private:
		bool drawGridTile = true;
	};
}