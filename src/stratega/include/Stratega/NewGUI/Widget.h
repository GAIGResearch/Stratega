#pragma once

#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct SGAWidget
	{
		virtual void update(const GameState& state) = 0;
		virtual void render(SGARenderTarget& renderTarget) = 0;
	};
}