#pragma once

#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/World.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct SGAWidget
	{
		SGAWidget(const std::string& newName, sf::RenderWindow& newWindow, World& newWorld):
			name(newName), window(newWindow), world(newWorld)
		{
		}
		virtual void update(const GameState& state) = 0;
		virtual void render(SGARenderTarget& renderTarget) = 0;

		bool enabled = true;
		const std::string name;
		sf::RenderWindow& window;
		World& world;
	};
}