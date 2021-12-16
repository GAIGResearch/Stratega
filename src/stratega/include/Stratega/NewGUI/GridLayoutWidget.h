#pragma once
#include <Stratega/NewGUI/Widget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct World;
	
	struct GridLayoutWidget : public SGAWidget
	{
		GridLayoutWidget(World& newWorld);

		void update(const GameState& state) override;

		void render(SGARenderTarget& renderTarget) override;

		bool drawCompleteGrid = false;
		World& world;
	};
}