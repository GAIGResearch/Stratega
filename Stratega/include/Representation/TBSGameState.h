#pragma once
#include <Representation/GameState.h>

namespace SGA
{
	struct TBSGameState : public GameState
	{
		TBSGameState() : GameState(), turnLimit(0), currentPlayer(0)
		{
		}

		TBSGameState(Board&& board, const std::unordered_map<int, TileType>& tileTypes)
			: GameState(std::move(board), tileTypes),
			turnLimit(1000000),
			currentPlayer(0)
		{
			
		}

		int turnLimit;
		int currentPlayer;
	};
}