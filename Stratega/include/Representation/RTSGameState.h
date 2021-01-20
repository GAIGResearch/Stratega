#pragma once
#include <Representation/GameState.h>
#include <Representation/Navigation.h>

namespace SGA
{
	struct RTSGameState : public GameState
	{
		std::shared_ptr<Navigation> navigation;

		RTSGameState():
			GameState()
		{
			
		}
		RTSGameState(Board&& board, const std::unordered_map<int, TileType>& tileTypes) :
			GameState(std::move(board), std::move(tileTypes)),
			navigation(nullptr)
		{

		}
	};
}