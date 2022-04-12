#pragma once
#include <boost/random.hpp>
#include <unordered_map>
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TileType.h>
#include <Stratega/Representation/GameInfo.h>
#include <memory>
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
	struct GameState
	{

	public:

		/***** CONSTRUCTORS *****/

		GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes);
		GameState();


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
		/// Returns a list with all the action types that a player can execute
		/// </summary>
		/// <param name="playerID">ID of the player we get the action types from.</param>
		/// <returns>Vector with action types this player can execute in this game</returns>
		std::vector<ActionType> getPlayerActionTypes(int playerID) const;

		/// <summary>
		/// Checks if given technology has been researched by a player.
		/// </summary>
		/// <param name="playerID">ID of the player to check</param>
		/// <param name="technologyID">ID of the technology to check</param>
		/// <returns>A boolean indicating if the player has researched the given technology</returns>
		bool isResearched(int playerID, int technologyID) const;

		/// <summary>
		/// Checks if given technology has been researched by a player, given the technology name
		/// </summary>
		/// <param name="playerID">ID of the player to check</param>
		/// <param name="technologyName">name of the technology to check</param>
		/// <returns>A boolean indicating if the player has researched the given technology</returns>
		bool isResearched(int playerID, const std::string& technologyName);

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

		/// <summary>
		/// Initializes the research technologies to all players, to none.
		/// </summary>
		void initResearchTechs();
		
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
		bool isInBounds(const Vector2i& pos) const;

		/// <summary>
		/// Checks if position is inside of the tile map
		/// </summary>
		/// <param name="pos">The position to be checked</param>
		/// <returns>A boolean indicating if tile map contains the position</returns>
		bool isInBounds(const Vector2f& pos) const;

		/// <summary>
		/// Returns the width of the board.
		/// </summary>
		/// <returns>The width of the board.</returns>
		int getBoardWidth() const { return static_cast<int>(board.getWidth()); }

		/// <summary>
		/// Returns the height of the board.
		/// </summary>
		/// <returns>The height of the board.</returns>
		int getBoardHeight() const { return static_cast<int>(board.getHeight()); }

		/// <summary>
		/// Returns the tile at the position indicated in the parameter. Can throw an exception if out of bounds.
		/// </summary>
		/// <param name="pos">Position of the tile to retrieve</param>
		/// <returns>The tile at 'pos'</returns>
		const Tile& getTileAt(const Vector2i& pos) const;

		/// <summary>
		/// Returns the tile at the position (x,y) indicated in the parameter. Can throw an exception if out of bounds.
		/// </summary>
		/// <param name="x">X Position of the tile to retrieve</param>
		/// <param name="y">Y Position of the tile to retrieve</param>
		/// <returns>The tile at position (x,y)</returns>
		const Tile& getTileAt(int x, int y) const;

		/// <summary>
		/// Initializes the board with the tiles passed by parameter. 
		/// </summary>
		/// <param name="tiles">Tiles to fill the board with.</param>
		/// <param name="boardWidth">Width of the board to initialize.</param>
		void initBoard(int boardWidth, std::vector<Tile>& tiles);


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
		/// Gets the list of all entities.
		/// </summary>
		/// <returns>A vector with all entities in the game.</returns>
		std::vector<Entity>& getEntities() { return entities; };
		const std::vector<Entity>& getEntities() const { return entities; };

		/// <summary>
		/// Gets the list of entities of the specified player.
		/// </summary>
		/// <param name="playerID">ID of the player whose entities are retrieved.</param>
		/// <param name="entityCategory">Entites retrieved will belong to this indicated category. If not suplied, this method returns all entities.</param>
		/// <returns>The list of entities of the given player. Returns an empty list if player ID doesn't exist or it has not entities.</returns>
		std::vector<Entity> getPlayerEntities(int playerID, EntityCategory entityCategory = EntityCategory::Null) const;

		/// <summary>
		/// Gets the list of entities that do not belong to the specified player.
		/// </summary>
		/// <param name="playerID">ID of the player whose entities are NOT to be retrieved.</param>
		/// <param name="entityCategory">Entites retrieved will belong to this indicated category. If not suplied, this method returns all entities.</param>
		/// <returns>The list of entities not own by the given player.</returns>
		std::vector<Entity> getNonPlayerEntities(int playerID, EntityCategory entityCategory = EntityCategory::Null) const;



		/***** PLAYERS FUNCTIONS *****/

		/// <summary>
		/// Gets a player given its ID.
		/// </summary>
		/// <param name="playerID">ID of the player to retrieve</param>
		/// <returns>Player seeked for, or nullptr if it doesn't exist.</returns>
		const Player* getPlayer(int playerID) const;
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		/// <summary>
		/// Returns a list with the ID of players that can play in this game state.
		/// </summary>
		/// <returns>A list with all IDs of player that can play now.</returns>
		std::vector<int> whoCanPlay() const;
		
		/// <summary>
		/// Indicates if the player with the provided ID can play in this game state.
		/// </summary>
		/// <param name="playerID">ID to check</param>
		/// <returns>true if the player with ID playerID can play now.</returns>
		bool canPlay(int playerID) const;

		/// <summary>
		/// Returns all players of this game.
		/// </summary>
		/// <returns>Players of the game.</returns>
		const std::vector<Player>& getPlayers() const { return players; };
		std::vector<Player>& getPlayers() { return players; };

		/// <summary>
		/// Gets the number of players in this game state.
		/// </summary>
		/// <returns>Number of players in this game</returns>
		int getNumPlayers() const { return static_cast<int>(players.size()); };

		/// <summary>
		/// Adds a player to the game state.
		/// </summary>
		/// <param name="p">The player to add.</param>
		/// <returns>Returns the ID of the player added to the game, equal to the number of players -1.</returns>
		int addPlayer(Player& p);

		/// <summary>
		/// Gets the value of a player parameter.
		/// Be sure to check first is the parameter you are asking for exist using <here cref="SGA::GameState::hasPlayerParameter()"/>
		/// </summary>
		/// <param name="playerID">ID of the player to get the parameter from</param>
		/// <param name="paramName">Name of the parameter which value is requested</param>
		/// <returns>Returns the value of the parameter indicated for the player whose ID is given.</returns>
		double getPlayerParameter(int playerID, const std::string& paramName) const;

		/// <summary>
		/// Indicates if the player has a specific parameter
		/// </summary>
		/// <param name="paramName">Name of the parameter which want to be checked</param>
		/// /// <returns>A bool indicating if player has that parameter</returns>
		bool hasPlayerParameter(const std::string& paramName) const;
		
		/// <summary>
		/// Returns a list will all the parameter names of the player of which ID is given
		/// </summary>
		/// <param name="playerID">ID of the player to get the parameter from. 
		/// For the moment, all players have the same parameter names (hence playerID is not used).</param>
		/// <returns>A vector with all the parameter names of the requested player.</returns>
		std::vector<std::string> getPlayerParameterNames(int playerID) const;

		/// <summary>
		/// Gets a map with all pairs <parameter,value>
		/// <summary>
		/// <param name="playerID">ID of the player to get the parameter from. </param>
		/// <returns>Returns a map with all the parameters of this player.</returns>
		std::unordered_map<std::string, double> getPlayerParameters(int playerID) const;

		/// <summary>
		/// Returns the score of the player whose ID is passed.
		/// </summary>
		/// <param name="playerID">ID of the player to pass.</param>
		/// <returns>The current score of the given player.</returns>
		int getPlayerScore(int playerID) const;



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
		void printActionInfo(const Action& action) const;

		/// <summary>
		/// Increments the ID for the next continuous action.
		/// </summary>
		void incNextContinuousActionID() { continuousActionNextID++; }

		/// <summary>
		/// Returns the ID for the next continuous action.
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
		/// Returns the ID of the player that moves in this state for Turn Based Games.
		/// For non-TBS, this value is -1.
		/// </summary>
		int getCurrentTBSPlayer() const { return currentPlayer; }

		/// <summary>
		/// Sets the current TBS player. For non-TBS, this should receive -1.
		/// </summary>
		void setCurrentTBSPlayer(int playerID) { currentPlayer = playerID; }

		/// <summary>
		/// Returns true if the game is over.
		/// </summary>
		/// <returns>Whether the game is over (true) or not (false).</returns>
		bool isGameOver() const { return gameOver; }

		/// <summary>
		/// Sets if the game is over
		/// </summary>
		/// <param name="over">Indicates if the game is over</param>
		void setGameOver(bool over) { gameOver = over; }

		/// <summary>
		/// Returns the player ID of the winner. If game is not over, this returns -1.
		/// </summary>
		/// <returns>ID of the winner player.</returns>
		int getWinnerID() const { return winnerPlayerID; }

		/// <summary>
		/// Sets the winner of the game.
		/// </summary>
		/// <param name="winnerID">Player ID of the winner</param>
		void setWinnerID(int winnerID) { winnerPlayerID = winnerID; }

		/// <summary>
		/// Returns the current tick of the game.
		/// </summary>
		int getCurrentTick() const { return currentTick; }
		
		/// <summary>
		/// Increments the current tick in the game by 1.
		/// </summary>
		void incTick() { currentTick++; }

		/// <summary>
		/// Returns the current game tick limit.
		/// </summary>
		int getTickLimit() { return tickLimit; }
		
		/// <summary>
		/// Sets the time limit of the game, measured in ticks.
		/// </summary>
		/// <param name="tickL"></param>
		void setTickLimit(int tickL) { tickLimit = tickL; }

		/// <summary>
		/// Returns a pointer to the struct with static information about the game.
		/// </summary>
		std::shared_ptr<GameInfo> getGameInfo() const { return gameInfo; }
		
		/// <summary>
		/// Sets the pointer to the game information struct.
		/// </summary>
		void setGameInfo(std::shared_ptr<GameInfo> gameInfoPtr) { gameInfo = gameInfoPtr; }

		/// <summary>
		/// Returns the type of the game, of GameType
		/// </summary>
		const GameType& getGameType() const { return gameType; }

		/// <summary>
		/// Sets the type of game (enum type GameType)
		/// </summary>
		void setGameType(GameType gt) { gameType = gt; }

		/// <summary>
		/// Returns a pointer to the Navigation object used by the RTS engine for pathfinding. 
		/// </summary>
		std::shared_ptr<Navigation> getRTSNavigation() const { return navigation; }
		
		/// <summary>
		/// Sets the pointer to the Navigation object used by the RTS engine for pathfinding. 
		/// </summary>
		void setRTSNavigation(std::shared_ptr<Navigation> nav) { navigation = nav; }

		/// <summary>
		/// Returns the game's random number generator.
		/// </summary>
		/// <returns></returns>
		boost::mt19937& getRndEngine() { return rngEngine; }


		/***** PARAMETERS *****/

		/// <summary>
		/// Returns the list of parameters, can't be modified.
		/// </summary>
		const std::vector<double>& getParameters() const { return parameters; }

		/// <summary>
		/// Returns the list of parameters of this player (can be modified)
		/// </summary>
		std::vector<double>& getParameters() { return parameters; }

		/// <summary>
		/// Returns a reference to a parameter value of this player.
		/// </summary>
		double& getRawParameterAt(int paramIdx) { return parameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>yer to a certain value
		/// </summary>
		/// Returns a const value of a parameter of this player.
		/// </summary>
		const double& getRawParameterAt(int paramIdx) const { return parameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Gets a specific parameters value, by index 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double getParameterAt(int paramIdx) { return parameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Gets a specific max parameters value, by index 
		/// <summary>
		/// <returns>The max parameter value.</returns>
		double getMaxParameterAt(int paramIdx) { return maxParameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Gets a specific min parameters value, by index 
		/// <summary>
		/// <returns>The min parameter value.</returns>
		double getMinParameterAt(int paramIdx) { return minParameters[static_cast<size_t>(paramIdx)]; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setParameter(int paramIdx, double val) { parameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setMaxParameter(int paramIdx, double val) { maxParameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets the parameter of this play
		/// <param name="paramIdx">Parameter index of this param.</param>
		/// <param name="val">Value to be set for the parameter.</param>
		void setMinParameter(int paramIdx, double val) { minParameters[static_cast<size_t>(paramIdx)] = val; }

		/// <summary>
		/// Sets a size for the vector of parameters of this player.
		/// </summary>
		void resizeParameters(int cap)
		{
			parameters.resize(static_cast<size_t>(cap));
			maxParameters.resize(static_cast<size_t>(cap));
			minParameters.resize(static_cast<size_t>(cap));
		}


	private:

		/// <summary>
		/// Indicates if the game is over.
		/// </summary>
		bool gameOver;

		/// <summary>
		/// The ID of the player who won the game if the game is over. Value is -1 if game not over yet. 
		/// </summary>
		int winnerPlayerID;

		/// <summary>
		/// Current turn (for turn based games) or frame (real-time) of the game.
		/// </summary>
		int currentTick;


		/// <summary>
		/// The id that will be assigned to the next entity that is added to the game.
		/// </summary>
		int nextEntityID = 0;

		/// <summary>
		/// Number of turns (TBS) or frames (RTS) when the game will finish.
		/// </summary>
		int tickLimit;

		/// <summary>
		/// Game Info (static information about this game)		
		/// </summary>
		std::shared_ptr<GameInfo> gameInfo;

		/// <summary>
		/// Type of game being played (TBS, RTS...), as defined <here cref="SGA::GameType"/>
		/// </summary>
		GameType gameType;

		/// <summary>
		/// Navigational Mesh (for RTS)	
		/// </summary>
		std::shared_ptr<Navigation> navigation;

		/// <summary>
		/// Tile used for fog of war.
		/// </summary>
		Tile fogOfWarTile;

		/// <summary>
		/// Board: a 2 dimensional grid of tiles. This does not contain information about entities on those tiles.		
		/// </summary>
		Grid2D<Tile> board;

		/// <summary>
		/// Map that indicates if a technology is researched by a player. Key is the player ID and maps to a vector of technology IDs.
		/// </summary>
		std::unordered_map<int, std::vector<int>> researchedTechnologies;

		/// <summary>
		/// List of entities in this game	
		/// </summary>
		std::vector<Entity> entities;


		/// <summary>
		/// List of players in this game	
		/// </summary>
		std::vector<Player> players;


		/// <summary>
		/// Random number generator engine
		/// </summary>
		boost::mt19937 rngEngine;

		/// <summary>
		/// ID of the fog of war tile.
		/// </summary>
		int fogOfWarId = -1;

		/// <summary>
		/// ID of the next continuous action to execute
		/// </summary>
		int continuousActionNextID = 0;

		/// <summary>
		/// ID of the current player if only one can play.
		/// 	-1 if more than one can play. Use gameState.whoCanPlay() to retrieve this in vector form with N-1 player IDs.
		/// </summary>
		int currentPlayer;

		/// <summary>
		/// Used to know if fog of war has been applied to this state
		/// </summary>
		bool fogOfWarApplied;

		/// <summary>
		/// List of parameter values.
		/// </summary>
		std::vector<double> parameters;

		/// <summary>
		/// Values for the max parameters value of this entity. Indexed by ID. Use getMaxParameter(...) functions to access these.
		/// </summary>
		std::vector<double> maxParameters;

		/// <summary>
		/// Values for the min parameters value of this entity. Indexed by ID. Use getMinParameter(...) functions to access these.
		/// </summary>
		std::vector<double> minParameters;
	};
}
