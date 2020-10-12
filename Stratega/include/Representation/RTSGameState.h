#pragma once
#include <unordered_map>
#include <Representation/Board.h>
#include <Representation/RTSUnit.h>
#include <Representation/Vector2.h>
#include <Representation/UnitType.h>
#include <Representation/Player.h>

namespace SGA
{
	class RTSPlayer;
	class RTSUnit;
	
	class RTSGameState
	{
	public:
		std::shared_ptr<std::unordered_map<int, UnitType>> unitTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;

		bool isGameOver;
		
		Board board;
		std::vector<RTSUnit> units;
		std::vector<RTSPlayer> players;

		int lastUsedPlayerID;
		int lastUsedUnitID;
		int winnerPlayerID;
		
		RTSGameState();
		RTSGameState(Board board, const std::unordered_map<int, UnitType>& unitTypes, const std::unordered_map<int, TileType>& tileTypes) :
			board(std::move(board)),
			unitTypes(std::make_shared<std::unordered_map<int, UnitType>>(unitTypes)),
			tileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes)),
			isGameOver(false),
			lastUsedPlayerID(-1),
			lastUsedUnitID(-1),
			winnerPlayerID(-1),
			tileScale(1)
		{
		}

		
		RTSGameState(const RTSGameState& copy) noexcept;
		RTSGameState(RTSGameState&& other) noexcept;
		RTSGameState& operator=(RTSGameState other) noexcept;

		friend void swap(RTSGameState& lhs, RTSGameState& rhs) noexcept
		{
			using std::swap;		
			swap(lhs.isGameOver, rhs.isGameOver);			
			swap(lhs.board, rhs.board);
			swap(lhs.units, rhs.units);
			swap(lhs.players, rhs.players);
			swap(lhs.lastUsedPlayerID, rhs.lastUsedPlayerID);
			swap(lhs.lastUsedUnitID, rhs.lastUsedUnitID);
			swap(lhs.winnerPlayerID, rhs.winnerPlayerID);			
			swap(lhs.tileScale, rhs.tileScale);			
			swap(lhs.unitTypes, rhs.unitTypes);
			swap(lhs.tileTypes, rhs.tileTypes);

			lhs.setOwner();
			rhs.setOwner();
		}
	
		
		std::vector<RTSUnit>& getUnits() { return units; }

		RTSUnit* getUnit(int unitID);
		RTSUnit* getUnit(Vector2f pos, float maxDistance = 1);

		
		[[nodiscard]] double getTileScale() const { return tileScale; }
		[[nodiscard]] const Board& getBoard() const { return board; }
		[[nodiscard]] Board& getBoard() { return board; }

		bool isInBounds(Vector2i pos) const
		{
			return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
		}

		int addPlayer();
		int addUnit(int playerID, int typeID, const Vector2i& position);
		void removeUnit(int id);

	private:
		double tileScale;
		void setOwner() noexcept;
	};
}
