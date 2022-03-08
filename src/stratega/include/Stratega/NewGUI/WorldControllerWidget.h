#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct WorldControllerWidget : public SGAWidget
	{
		WorldControllerWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFM);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

	};
}