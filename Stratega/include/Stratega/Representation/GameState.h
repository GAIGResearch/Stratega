#pragma once
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <Stratega/Representation/Grid2D.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/EntityType.h>
#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/TileType.h>
namespace SGA
{
	struct GameState
	{
		GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes);

		GameState();

		int continueActionNextID = 0;
		//Rule of six
		virtual ~GameState() = default;
		GameState(const GameState& other) = default;
		GameState(GameState&& other) noexcept = default;
		GameState& operator=(const GameState& other) = default;
		GameState& operator=(GameState&& other) noexcept = default;

		// Type information
		std::shared_ptr<std::unordered_map<std::string, ParameterID>> parameterIDLookup;
		std::shared_ptr<std::unordered_map<ParameterID, Parameter>> playerParameterTypes;
		std::shared_ptr<std::unordered_set<EntityTypeID>> playerSpawnableTypes;
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;
		
		//Technology tree
		std::shared_ptr <TechnologyTreeCollection> technologyTreeCollection;		

		std::unordered_map<std::string, std::unordered_set<EntityTypeID>> entityGroups;

		std::mt19937 rngEngine;

		// Game information
		bool isGameOver;
		int winnerPlayerID;
		int currentTick;
		int tickLimit;
		
		// Board information
		Tile fogOfWarTile;
		int fogOfWarId = -1;
		Grid2D<Tile> board;

		// Player and unit information
		std::vector<Entity> entities;
		std::vector<Player> players;
		int nextEntityID;
		int nextPlayerID;

		virtual bool canExecuteAction(Entity& entity, const ActionType& actionType);
		virtual bool canExecuteAction(Player& player, const ActionType& actionType);

		const Entity* getEntityAt(const Vector2f& pos) const;
		
		Entity* getEntity(int entityID);

		const Entity& getEntityConst(int entityID) const;

		const EntityType& getEntityType(int entityTypeID) const;
		
		const ActionType& getActionTypeConst(int actionTypeID);
		
		int getParameterGlobalID(std::string parameterName);

		int getActionTypeID(std::string parameterName);

		const SGA::Parameter& getPlayerParameter(ParameterID id) const;
		
		Entity* getEntity(Vector2f pos, float maxDistance);
		
		const Parameter& getParameterType(int entityTypeID, int globalParameterID) const;
		
		bool checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const;
		
		int addPlayer(std::vector<int> actionIds);

		int addEntity(const EntityType& type, int playerID, const Vector2f& position);

		Entity* getEntity(Vector2f pos);

		bool isWalkable(const Vector2i& position);
		
		//bool isTile(const Vector2i& position, std::string tileName);
		const TileType& getTileType(int tileTypeID) const
		{
			return tileTypes->find(tileTypeID)->second;
		}
		
		bool isInBounds(Vector2i pos);
				
		const ActionType& getActionType(int typeID) const;

		bool isInBounds(Vector2f pos) const;

		// Dirty trick to reuse code between const and non-const getter
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		const Player* getPlayer(int playerID) const;

		std::vector<const Entity*> getPlayerEntities(int playerID) const;

		std::vector< Entity*> getPlayerEntities(int playerID);

		void applyFogOfWar(int playerID);

		void setRNGEngine(std::mt19937 engine)
		{
			rngEngine = engine;
		}

		const std::mt19937& getRNGEngine() const
		{
			return rngEngine;
		}
	};
}
