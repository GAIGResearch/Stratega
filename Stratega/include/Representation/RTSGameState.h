#pragma once
#include <Representation/GameState.h>
#include <Representation/Navigation.h>

namespace SGA
{
	struct RTSGameState2 : public GameState
	{
		std::shared_ptr<Navigation> navigation;

		RTSGameState2():
			GameState()
		{
			
		}
		RTSGameState2(Board&& board, const std::unordered_map<int, TileType>& tileTypes) :
			GameState(std::move(board), std::move(tileTypes)),
			navigation(nullptr)
		{

		}
	};
}