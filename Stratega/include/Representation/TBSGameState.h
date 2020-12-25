#pragma once
#include <Representation/GameState.h>

namespace SGA
{
	struct TBSGameState : public GameState
	{
		TBSGameState() : GameState(), currentGameTurn(0), turnLimit(0), currentPlayer(0)
		{
		}

		TBSGameState(Board&& board, const std::unordered_map<int, TileType>& tileTypes)
			: GameState(std::move(board), tileTypes),
			currentGameTurn(0),
			turnLimit(1000000),
			currentPlayer(0)
		{
			
		}

		int currentGameTurn;
		int turnLimit;
		int currentPlayer;
	};
}