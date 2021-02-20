#pragma once
#include <Stratega/Representation/GameState.h>

namespace SGA::Widgets
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
		FogRenderType renderType = FogRenderType::Nothing;
		int selectedPlayerID = -1;
	};

	/// <summary>
	/// Displays a widget for changing the render-settings of FogOfWar
	/// </summary>
	/// <param name="state">The current GameState</param>
	/// <param name="fowSettings">The settings that will be rendered</param>
	/// <returns>A boolean indicating if the selected player changed</returns>
	bool fowController(const GameState& state, FogOfWarSettings& fowSettings);
}
