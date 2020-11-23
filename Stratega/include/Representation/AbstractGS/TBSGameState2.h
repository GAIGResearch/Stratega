#pragma once
#include <Representation/AbstractGS/GameState.h>

namespace SGA
{
	struct TBSGameState2 : public GameState
	{
		int currentGameTurn;
		int turnLimit;
		int currentPlayer;
	};
}