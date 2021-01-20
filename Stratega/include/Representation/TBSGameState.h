#pragma once
#include <Representation/GameState.h>

namespace SGA
{
	struct TBSGameState : public GameState
	{
		TBSGameState() : GameState(), currentPlayer(0)
		{
		}

		TBSGameState(Board&& board, const std::unordered_map<int, TileType>& tileTypes)
			: GameState(std::move(board), tileTypes),
			currentPlayer(0)
		{
			
		}

		int currentPlayer;
	};
}