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

		bool canExecuteAction(const Entity& entity, const ActionType& actionType) const;
		bool canExecuteAction(const Player& player, const ActionType& actionType) const;

		//Entities
		const Entity* getEntityAt(const Vector2f& pos) const;
		
		Entity* getEntity(int entityID);

		const Entity* getEntityConst(int entityID) const;

		Entity* getEntity(Vector2f pos, float maxDistance);

		int addEntity(const EntityType& type, int playerID, const Vector2f& position);

		Entity* getEntity(Vector2f pos);

		
		//Grid
		bool isWalkable(const Vector2i& position);
		
		bool isInBounds(Vector2i pos);		

		bool isInBounds(Vector2f pos) const;


		//Player
		// Dirty trick to reuse code between const and non-const getter
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		const Player* getPlayer(int playerID) const;

		std::vector<const Entity*> getPlayerEntities(int playerID) const;

		std::vector< Entity*> getPlayerEntities(int playerID);

		int addPlayer(std::vector<int> actionIds);

		
		//Fog of war
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

		//Print state info

		void printStateInfo();

		void printBoard()
		{
			std::string map;
			std::cout << "---------[Board]---------" << std::endl;
			for (int y = 0; y < board.getHeight(); ++y)
			{
				for (int x = 0; x < board.getWidth(); ++x)
				{
					//Get tile type
					map+=  gameInfo->getTileType(board.get(x, y).tileTypeID).symbol + "  ";
				}
				std::cout << std::endl;
			}

			for (int y = 0; y < board.getHeight(); ++y)
			{
				for (int x = 0; x < board.getWidth(); ++x)
				{
					//Get tile type
					map += gameInfo->getTileType(board.get(x, y).tileTypeID).symbol + "  ";
				}
				std::cout << std::endl;
			}
		}

		//Print info
		void printEntityInfo(int entityID)
		{
			std::cout << "---------[EntityInfo]---------" << std::endl;
			auto* entity = getEntity(entityID);

			if(entity)
			{
				std::cout << "[OwnerID]: " << entity->id << std::endl;
				std::cout << "	[type]: " << gameInfo->getEntityType(entity->typeID).name << std::endl;
				std::cout << "	[position]: " << entity->position.x<<","<<entity->position.y << std::endl;
				std::cout << "	[parameters]: " << std::endl;

				int parameterID=0;
				auto& entityType = gameInfo->getEntityType(entity->typeID);
				for (auto& parameter : entity->parameters)
				{
					std::cout << "		["<<entityType.parameters.find(parameterID++)->second.name<<"]: " << parameter << std::endl;
				}
				
			}
			else
			{
				std::cout << "EntityNotfound"<<std::endl;
			}
		}
		
	private:
		int nextPlayerID;
		int nextEntityID;
	};
}
