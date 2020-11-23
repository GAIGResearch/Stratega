#pragma once
#include <Representation/AbstractGS/GameState.h>
#include <Representation/Navigation.h>

namespace SGA
{
	struct RTSGameState2 : public GameState
	{
		std::shared_ptr<Navigation> navigation;
	};
}