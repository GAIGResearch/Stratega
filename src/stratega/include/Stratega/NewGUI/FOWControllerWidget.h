#pragma once
#include <Stratega/NewGUI/FogOfWarSettings.h>
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	

	struct World;
	
	struct FOWControllerWidget : public SGAWidget
	{
		FOWControllerWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm, FogOfWarSettings* newSettings);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

	private:
		FogOfWarSettings* fowSettings;
		const GameState* currentState;
		bool renderFogOfWarBefore = false;
	};
}