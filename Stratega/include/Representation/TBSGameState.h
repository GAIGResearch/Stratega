#pragma once
#include <Representation/Board.h>
#include <Representation/Player.h>
#include <Representation/TBSUnit.h>
#include <Representation/UnitType.h>
#include <Representation/TileType.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>

namespace SGA
{
	class TBSPlayer;
	class TBSUnit;
	
	class TBSGameState
	{
	public:
		Tile fogOfWarTile;
		bool isGameOver = false;
		int currentGameTurn;
		int currentPlayer;

		int roundLimit=1;

		int fogOfWarId = -1;

		TBSGameState();
		TBSGameState(Board&& board, const std::unordered_map<int, UnitType>& unitTypes, const std::unordered_map<int, TileType>& tileTypes);
		// Rule of Five - Needed to enable circular references
		TBSGameState(const TBSGameState& copy) noexcept;
		TBSGameState(TBSGameState&& other) noexcept;
		TBSGameState& operator=(TBSGameState other) noexcept;
		// GameState& operator=(GameState&& other) = delete; Idk why but even if i delete this constructor, the compiler throws ambiguous overload errors, so no rule of five...
		~TBSGameState() = default;
		friend void swap(TBSGameState& lhs, TBSGameState& rhs) noexcept
		{
			using std::swap;
			swap(lhs.fogOfWarTile, rhs.fogOfWarTile);
			swap(lhs.isGameOver, rhs.isGameOver);
			swap(lhs.currentGameTurn, rhs.currentGameTurn);
			swap(lhs.currentPlayer, rhs.currentPlayer);
			swap(lhs.board, rhs.board);
			swap(lhs.units, rhs.units);
			swap(lhs.players, rhs.players);
			swap(lhs.lastUsedPlayerID, rhs.lastUsedPlayerID);
			swap(lhs.lastUsedUnitID, rhs.lastUsedUnitID);
			swap(lhs.winnerPlayerID, rhs.winnerPlayerID);
			swap(lhs.roundLimit, rhs.roundLimit);
			swap(lhs.unitTypes, rhs.unitTypes);
			swap(lhs.tileTypes, rhs.tileTypes);

			lhs.setOwner();
			rhs.setOwner();
		}

		int addUnit(int playerID, int typeID, const Vector2i& position);
		int addPlayer();

		void removeUnit(int unitID);

		TBSUnit* getUnit(int unitID);
		TBSUnit* getUnit(Vector2i pos);
		void setWinnerID(int winner) { winnerPlayerID = winner; }

		Board& getBoard() { return board; }
		const Board& getBoard() const { return board; }
		std::vector<TBSUnit>& getUnits() { return units; }
		std::vector<TBSPlayer>& getPlayers() { return players; }
		TBSPlayer* getPlayer(int playerID);
		int getWinnerID() const { return winnerPlayerID; }
		int getPlayerNumber() const {return (int)players.size();}
		const std::unordered_map<int, UnitType>& getUnitTypes() const { return *unitTypes; }
		const std::unordered_map<int, TileType>& getTileTypes() const { return *tileTypes; }

		void printGameStateStatus();

		bool isInBounds(Vector2i pos)
		{
			return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
		}

		void applyFogOfWar(int playerID);
		
	private:
		Board board;
		std::vector<TBSUnit> units;
		std::vector<TBSPlayer> players;

		int lastUsedPlayerID;
		int lastUsedUnitID;
		int winnerPlayerID;

		std::shared_ptr<std::unordered_map<int, UnitType>> unitTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;
		
		// This method is used to set the gamestate references of all child-objects to this
		// Which is normally not necessary, and only used to implement the swap operation
		void setOwner() noexcept;
	};

}
