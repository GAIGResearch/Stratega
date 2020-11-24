#pragma once
#include <Representation/AbstractGS/EntityType.h>
#include <ForwardModel/AbstractFM/ActionType.h>
#include <Representation/AbstractGS/Entity.h>
#include <Representation/AbstractGS/Player.h>
#include <Representation/Board.h>

namespace SGA
{
	struct GameState
	{
		// Rule of five

		GameState():
			board(0,0),
			fogOfWarTile(0,0,0)

		{

		}
		virtual ~GameState() = default;
		GameState(const GameState& other) = default;
		GameState(GameState&& other) noexcept = default;
		GameState& operator=(const GameState& other) = default;
		GameState& operator=(GameState&& other) noexcept = default;

		// Type information
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;

		// Game information
		bool isGameOver;
		int winnerPlayerID;
		
		// Board information
		Tile fogOfWarTile;
		int fogOfWarId = -1;
		Board board;

		// Player and unit information
		std::vector<Entity> entities;
		std::vector<Player> players;
		int lastUsedEntityID;
		int lastUsedPlayerID;
	};
}