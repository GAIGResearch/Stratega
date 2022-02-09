#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	enum class FogRenderType
	{
		Nothing,
		Fog,
		Tiles
	};

	struct FogOfWarSettings
	{
		bool renderFogOfWar = false;
		SGA::FogRenderType renderType;
		int selectedPlayerID = -1;

		bool settingsChanged = false;
	};
}