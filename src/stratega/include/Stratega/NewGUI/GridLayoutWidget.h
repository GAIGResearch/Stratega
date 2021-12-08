#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct GridLayout : public SGAWidget
	{
		GridLayout(World& newWorld);

		void update(const GameState& state) override;

		void render(SGARenderTarget& renderTarget) override;

		World& world;
	};
}