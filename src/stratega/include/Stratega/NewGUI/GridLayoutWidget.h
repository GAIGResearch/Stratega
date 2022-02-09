#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct GridLayoutWidget : public SGAWidget
	{
		GridLayoutWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* fm);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

		//Custom information
		bool drawCompleteGrid = false;
	};
}