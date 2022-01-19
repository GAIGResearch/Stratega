#pragma once
#include <Stratega/Representation/GameState.h>

#include <Stratega/NewGUI/FogOfWarSettings.h>
namespace Widgets
{	
	/// <summary>
	/// Displays a widget for changing the render-settings of FogOfWar
	/// </summary>
	/// <param name="state">The current GameState</param>
	/// <param name="fowSettings">The settings that will be rendered</param>
	/// <returns>A boolean indicating if the selected player changed</returns>
	bool fowController(const SGA::GameState& state, SGA::FogOfWarSettings& fowSettings);
}
