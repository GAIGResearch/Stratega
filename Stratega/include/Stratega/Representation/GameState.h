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

	public:

		GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes);
		GameState();

		/// <summary>
		/// Game Info (static information about this game)		
		/// </summary>
		std::shared_ptr<GameInfo> gameInfo;

		/// <summary>
		/// ID of the current player (TBS only)		
		/// </summary>
		int currentPlayer;

		/// <summary>
		/// Navigational Mesh (for RTS)	
		/// </summary>
		std::shared_ptr<Navigation> navigation;

		/// <summary>
		/// Type of game being played (TBS, RTS...), as defined <here cref="SGA::GameType"/>
		/// </summary>
		GameType gameType;

		/// <summary>
		/// Indicates if the game is over.
		/// </summary>
		bool isGameOver;

		/// <summary>
		/// The ID of the player who won the game if the game is over. Value is -1 if game not over yet. 
		/// </summary>
		int winnerPlayerID;

		/// <summary>
		/// Current turn (for turn based games) or frame (real-time) of the game.
		/// </summary>
		int currentTick;

		/// <summary>
		/// Number of turns (TBS) or frames (RTS) when the game will finish.
		/// </summary>
		int tickLimit;

		/// <summary>
		/// Board: a 2 dimensional grid of tiles. This does not contain information about entities on those tiles.		
		/// </summary>
		Grid2D<Tile> board;

		/// <summary>
		/// List of entities in this game	
		/// </summary>
		std::vector<Entity> entities;

		/// <summary>
		/// List of players in this game	
		/// </summary>
		std::vector<Player> players;

		/// <summary>
		/// Map that indicates if a technology is researched by a player. Key is the player ID and maps to a vector of technology IDs.
		/// </summary>
		std::unordered_map<int, std::vector<int>> researchedTechnologies;
		



		/***** ACTIONS AND RESEARCH FUNCTIONS *****/

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

		/// <summary>
		/// Checks if given technology has been researched by a player.
		/// </summary>
		/// <param name="playerID">ID of the player to check</param>
		/// <param name="technologyID">ID of the technology to check</param>
		/// <returns>A boolean indicating if the player has researched the given technology</returns>
		bool isResearched(int playerID, int technologyID) const;

		/// <summary>
		/// Checks if given technology can been researched by a player.
		/// </summary>
		/// <param name="playerID">ID of the player to check</param>
		/// <param name="technologyID">ID of the technology to check</param>
		/// <returns>A boolean indicating if the player can research the given technology</returns>
		bool canResearch(int playerID, int technologyID) const;

		/// <summary>
		/// Process the research of a given technology for a player. This method does not check
		/// if the technology is research or if it can be researched.
		/// </summary>
		/// <param name="playerID">ID of the player who has to research a tech</param>
		/// <param name="technologyID">ID of the technology to research</param>
		void researchTechnology(int playerID, int technologyID);




		/***** BOARD FUNCTIONS *****/

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





		/***** GAME ENTITIES FUNCTIONS *****/

		/// <summary>
		/// Returns an entity at board position 'pos'. It'll return a nullptr if no entities at this position. 
		/// </summary>
		/// <param name="pos">Position in the board to look for an entity.</param>
		/// <param name="maxDistance">If provided, considers units at a distance less or equal this value to the position provided.</param>
		/// <returns>A pointer to the entity in this location.</returns>
		Entity* getEntity(Vector2f pos, float maxDistance = 0.0);
		const Entity* getEntityAt(const Vector2f& pos) const;

		/// <summary>
		/// Returns an entity by its ID. It'll return nullptr if no entity exists associated to the given ID.
		/// </summary>
		/// <param name="entityID"> ID of the entity to retrieve. </param>
		/// <returns>A pointer to the entity.</returns>
		Entity* getEntity(int entityID);
		const Entity* getEntityConst(int entityID) const;


		/// <summary>
		/// Adds a new entity of a given type to the game, in a given position, belonging to a specific player. 
		/// </summary>
		/// <param name="type">Type of the player, as defined <here cref="SGA::EntityType"/> </param>
		/// <param name="playerID">ID of the player this new entity will belong to.</param>
		/// <param name="position">Position where the entity will be added.</param>
		/// <returns>Returns the unique ID of the entity created.</returns>
		int addEntity(const EntityType& type, int playerID, const Vector2f& position);

		/// <summary>
		/// Gets the list of entities of the specified player.
		/// </summary>
		/// <param name="playerID">ID of the player whose entities are retrieved.</param>
		/// <returns>The list of entities of the given player. Returns an empty list if player ID doesn't exist or it has not entities.</returns>
		//std::vector< Entity*> getPlayerEntities(int playerID);
		std::vector<const Entity*> getPlayerEntities(int playerID) const;

		/// <summary>
		/// Gets the list of entities of the specified player.
		/// </summary>
		/// <param name="playerID">ID of the player whose entities are retrieved.</param>
		/// <param name="entityCategory">Entites retrieved will belong to this indicated category. If not suplied, this method returns all entities.</param>
		/// <returns>The list of entities of the given player. Returns an empty list if player ID doesn't exist or it has not entities.</returns>
		std::vector< Entity*> getPlayerEntities(int playerID, EntityCategory entityCategory = EntityCategory::Null);

		/// <summary>
		/// Gets the list of entities that do not belong to the specified player.
		/// </summary>
		/// <param name="playerID">ID of the player whose entities are NOT to be retrieved.</param>
		/// <param name="entityCategory">Entites retrieved will belong to this indicated category. If not suplied, this method returns all entities.</param>
		/// <returns>The list of entities not own by the given player.</returns>
		std::vector< Entity*> getNonPlayerEntities(int playerID, EntityCategory entityCategory = EntityCategory::Null);




		/***** PLAYERS FUNCTIONS *****/

		/// <summary>
		/// Gets a player given its ID.
		/// </summary>
		/// <param name="playerID">ID of the player to retrieve</param>
		/// <returns>Player seeked for, or nullptr if it doesn't exist.</returns>
		const Player* getPlayer(int playerID) const;
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		/// <summary>
		/// Gets the number of players in this game state.
		/// </summary>
		/// <returns>Number of players in this game</returns>
		int getNumPlayers() const { return players.size(); };

		/// <summary>
		/// Adds a player to the game state.
		/// </summary>
		/// <param name="p">The player to add.</param>
		/// <returns>Returns the ID of the player added to the game, equal to the number of players -1.</returns>
		int addPlayer(Player p);




		/***** PRINT FUNCTIONS *****/

		/// <summary>
		/// Print all the entities of the current state
		/// </summary>
		void printStateInfo() const;
		
		/// <summary>
		// /Print view of the map of the current state
		/// </summary>
		void printBoard() const;
		
		/// <summary>
		/// Print view of the map of the current state applying fog
		/// <param name="playerID">The ID of the player to print information of.</param>
		/// </summary>
		void printBoard(int playerID) const;
		
		/// <summary>
		/// Print information of a specific entity
		/// <param name="playerID">ID of the entity to print information of.</param>
		/// </summary>
		void printEntityInfo(int entityID) const;
		
		/// <summary>
		/// Print information of a specific action
		/// </summary>
		void printActionInfo(Action& action) const;

		/// <summary>
		/// Increments the ID for the next continuous action.
		/// </summary>
		void incNextContinuousActionID() { continuousActionNextID++; }

		/// <summary>
		/// Return the ID for the next continuous action.
		/// </summary>
		/// <returns>The ID of the next continuous action.</returns>
		int getNextContinuousActionID() { return continuousActionNextID; }




		/***** MISC UTILITIES *****/

		/// <summary>
		/// Removes entities and hide tiles that are not visible from the point of view of the given player.
		/// </summary>
		/// <param name="playerID">The playerID of the player from which the fow will be applied</param>
		void applyFogOfWar(int playerID);

		/// <summary>
		/// Returns the ID of the tile that represents the fog of war.
		/// </summary>
		/// <returns>ID of the tile that represents the fog of war</returns>
		int getFogOfWarTileId() const { return fogOfWarId; }

		/// <summary>
		/// Random number generator engine
		/// </summary>
		std::mt19937 rngEngine;

	private:

		/// <summary>
		/// Tile used for fog of war.
		/// </summary>
		Tile fogOfWarTile;

		/// <summary>
		/// ID of the fog of war tile.
		/// </summary>
		int fogOfWarId = -1;

		/// <summary>
		/// ID of the next continuous action to execute
		/// </summary>
		int continuousActionNextID = 0;

	};
}
