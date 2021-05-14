#pragma once
#include <random>
#include <unordered_map>
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TileType.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{
	enum class GameType
	{
		TBS,
		RTS
	};
	/// <summary>
	/// Contains the game data without any logic, offering access to the current board, a list of player and their units.
	/// If the agent want access to the definition of entity types, actions or game config yaml  it should access to <see cref="SGA::GameInfo"/>
	/// </summary>
	struct GameState final
	{
		GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes);
		GameState();

		// Game Info
		std::shared_ptr<GameInfo> gameInfo;
		
		// TBS related data
		int currentPlayer;

		// RTS related data
		std::shared_ptr<Navigation> navigation;

		//Random engine
		std::mt19937 rngEngine;

		// Game information
		GameType gameType;
		bool isGameOver;
		int winnerPlayerID;
		int currentTick;
		int tickLimit;
		int continueActionNextID = 0;
		
		// Board information
		Tile fogOfWarTile;
		int fogOfWarId = -1;
		Grid2D<Tile> board;

		// Player and unit information
		std::vector<Entity> entities;
		std::vector<Player> players;

		/// <summary>
		/// Checks if a <see cref="SGA::Entity"/> can execute a given actionType
		/// </summary>
		/// <param name="entity">The entity that will be analyzed</param>
		/// <returns>A boolean indicating if the player can execute the action type</returns>
		bool canExecuteAction(const Entity& entity, const ActionType& actionType) const;
		/// <summary>
		/// Checks if a player can execute a given actionType
		/// </summary>
		/// <param name="player">The player that will be analyzed</param>
		/// <returns>A boolean indicating if the entity can execute the action type</returns>
		bool canExecuteAction(const Player& player, const ActionType& actionType) const;

		//Entities
		const Entity* getEntityAt(const Vector2f& pos) const;
		
		Entity* getEntity(int entityID);

		const Entity* getEntityConst(int entityID) const;

		Entity* getEntity(Vector2f pos, float maxDistance);

		int addEntity(const EntityType& type, int playerID, const Vector2f& position);

		Entity* getEntity(Vector2f pos);

		
		//Grid
		/// <summary>
		/// Checks if tile is occupied or the tile is walkable
		/// </summary>
		/// <param name="position">The position of the tile map</param>
		/// <returns>A boolean indicating if the tile in the given position is walkable</returns>
		bool isWalkable(const Vector2i& position);

		/// <summary>
		/// Checks if position is inside of the tile map
		/// </summary>
		/// <param name="pos">The position to be checked</param>
		/// <returns>A boolean indicating if tile map contains the position</returns>
		bool isInBounds(Vector2i pos) const;

		/// <summary>
		/// Checks if position is inside of the tile map
		/// </summary>
		/// <param name="pos">The position to be checked</param>
		/// <returns>A boolean indicating if tile map contains the position</returns>
		bool isInBounds(Vector2f pos) const;


		//Player
		// Dirty trick to reuse code between const and non-const getter
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		const Player* getPlayer(int playerID) const;

		std::vector<const Entity*> getPlayerEntities(int playerID) const;

		std::vector< Entity*> getPlayerEntities(int playerID);

		int addPlayer(std::vector<int> actionIds);

		/// <summary>
		/// Removes entities and hide tiles that are not visible from the point of view of the given player.
		/// </summary>
		/// <param name="playerID">The playerID of the player from which the fow will be applied</param>
		void applyFogOfWar(int playerID);


		//Random engines
		void setRNGEngine(std::mt19937 engine)
		{
			rngEngine = engine;
		}

		const std::mt19937& getRNGEngine() const
		{
			return rngEngine;
		}


		//Print all the entities of the current state
		void printStateInfo() const;
		//Print view of the map of the current state
		void printBoard() const;
		//Print view of the map of the current state applying fog
		void printBoard(int playerID) const;
		//Print information of a specific entity
		void printEntityInfo(int entityID);
		//Print information of a specific action
		void printActionInfo(Action& action) const;
		
	private:
		int nextPlayerID;
		int nextEntityID;
	};
}
