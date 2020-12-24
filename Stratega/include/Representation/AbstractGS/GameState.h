#pragma once
#include <Representation/AbstractGS/EntityType.h>
#include <ForwardModel/AbstractFM/ActionType.h>
#include <Representation/AbstractGS/Entity.h>
#include <Representation/AbstractGS/Player.h>
#include <Representation/Board.h>
//#include <ForwardModel/AbstractFM/Precondition.h>

namespace SGA
{
	struct GameState
	{
		GameState::GameState(Board&& board, const std::unordered_map<int, TileType>& tileTypes) :
			parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
			entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
			actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
			tileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes)),
			isGameOver(false),
			winnerPlayerID(-1),
			fogOfWarTile(-1, 0, 0),
			board(std::move(board)),
			players(),
			lastUsedEntityID(0),
			lastUsedPlayerID(-1)
		{
		}

		GameState()
			: parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
			  entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
			  actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
			  tileTypes(std::make_shared<std::unordered_map<int, TileType>>()),
			  isGameOver(false),
			  winnerPlayerID(0),
			  fogOfWarTile(0,0,0),
			  fogOfWarId(0),
			  board(10,10),
			  lastUsedEntityID(0),
			  lastUsedPlayerID(0)
		{
		}

		//Rule of six
		virtual ~GameState() = default;
		GameState(const GameState& other) = default;
		GameState(GameState&& other) noexcept = default;
		GameState& operator=(const GameState& other) = default;
		GameState& operator=(GameState&& other) noexcept = default;

		// Type information
		std::shared_ptr<std::unordered_map<std::string, ParameterID>> parameterIDLookup;
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;
		std::unordered_map<std::string, std::vector<int>> entityGroups;
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

		virtual bool canExecuteAction(Entity& entity, ActionType& actionType);

		Entity& getEntity(int entityID)
		{
			return* std::find_if(std::begin(entities), std::end(entities),
				[&](Entity const& p) { return p.id == entityID; });			
		}

		const Entity& getEntityConst(int entityID) const 
		{
			return *std::find_if(std::begin(entities), std::end(entities),
				[&](Entity const& p) { return p.id == entityID; });
		}

		const EntityType& getEntityType(int entityTypeID) const
		{
			auto it = entityTypes->find(entityTypeID);
			if (it != entityTypes->end())
			{
				return it->second;
			}
			else
			{
				std::string s;
				s.append("Tried accessing unknown entity type with ID=");
				s.append(std::to_string(entityTypeID));
				throw std::runtime_error(s);
			}
		}
		
		int getParameterGlobalID(std::string parameterName)
		{
			int foundId = -1;
			auto  iterator=parameterIDLookup->find(parameterName);
			
			if (iterator != parameterIDLookup->end())
				foundId = iterator->second;
			
			return foundId;
		}
		
		Entity* getEntity(Vector2f pos, float maxDistance)
		{
			for (auto& unit : entities)
			{
				if (unit.position.distance(pos) <= maxDistance)
				{
					return &unit;
				}
			}
			return nullptr;
		}
		
		Parameter getParameterType(int entityTypeID, int globalParameterID) const
		{
			auto& entityType = getEntityType(entityTypeID);

			return entityType.parameters.find(globalParameterID)->second;
		}
		
		double& getParameterReference(int entityID,int globalParameterID)
		{
			auto& entity = getEntity(entityID);
			auto& entityType = getEntityType(entity.typeID);

			int parameterIndex=entityType.parameters.find(globalParameterID)->second.index;
			
			return entity.parameters[parameterIndex];
		}
		
		bool checkEntityHaveParameter(int entityTypeID,std::string parameterName)
		{
			auto& entityType=getEntityType(entityTypeID);
			for (auto& parameter : entityType.parameters)
			{
				if (parameter.second.name == parameterName)
					return true;
			}
			
			return false;
		}
		
		int addPlayer()
		{
			lastUsedPlayerID++;
			players.emplace_back(lastUsedPlayerID,*this);
			return lastUsedPlayerID;
		}

		Entity* getEntity(SGA::Vector2f pos)
		{
			for (auto& entity : entities)
			{
				if (entity.position == pos)
					return &entity;
			}

			return nullptr;
		}

		bool isWalkable(const Vector2i& position)
		{
			Tile& targetTile = board.getTile(position.x, position.y);
			Entity* targetUnit = getEntity(position);

			return targetUnit == nullptr && targetTile.isWalkable;
		}

		
		bool isInBounds(Vector2i pos)
		{
			return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
		}

		ActionType& getActionType(int typeID)
		{
			return actionTypes->find(typeID)->second;
		}

		bool isInBounds(Vector2f pos) const
		{
			return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
		}

		const Player* getPlayer(int playerID) const
		{
			for(const auto& p : players)
			{
				if (p.id == playerID)
					return &p;
			}

			return nullptr;
		}
	};
}