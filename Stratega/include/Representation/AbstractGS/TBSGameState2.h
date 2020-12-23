#pragma once
#include <Representation/AbstractGS/GameState.h>

namespace SGA
{
	struct TBSGameState2 : public GameState
	{
		TBSGameState2()
		:
			GameState()
		{
			
		}
		TBSGameState2(Board&& board, const std::unordered_map<int, TileType>& tileTypes)
			:GameState(std::move(board),std::move(tileTypes)),
			currentGameTurn(0),
			currentPlayer(0),
			turnLimit(1000000)
		{
			
		}

		int currentGameTurn;
		int turnLimit;
		int currentPlayer;
	};
}