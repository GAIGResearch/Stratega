#pragma once
#include <Representation/EntityType.h>
#include <ForwardModel/ActionType.h>
#include <Representation/Entity.h>
#include <Representation/Player.h>
#include <Representation/Grid2D.h>
#include <Representation/TechnologyTree.h>
#include <Representation/Tile.h>
#include <Representation/TileType.h>

namespace SGA
{
	struct GameState
	{
		GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes) :
			parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
			entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
			actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
			tileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes)),
			technologyTreeCollection(std::make_shared<TechnologyTreeCollection>()),
			isGameOver(false),
			winnerPlayerID(-1),
			currentTick(1),
			tickLimit(-1),
			fogOfWarTile(-1, 0, 0),
			fogOfWarId(-1),
			board(std::move(board)),
			players(),
			nextEntityID(0),
			nextPlayerID(0)
		{
		}

		GameState()
			: parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
			  entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
			  actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
			  tileTypes(std::make_shared<std::unordered_map<int, TileType>>()),
			  technologyTreeCollection(std::make_shared<TechnologyTreeCollection>()),
			  isGameOver(false),
			  winnerPlayerID(-1),
			  currentTick(1),
			  tickLimit(-1),
			  fogOfWarTile(-1,0,0),
			  fogOfWarId(-1),
			  board(0,0, fogOfWarTile),
			  nextEntityID(0),
			  nextPlayerID(0)
		{
		}

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
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;
		
		//Technology tree
		std::shared_ptr <TechnologyTreeCollection> technologyTreeCollection;		

		std::unordered_map<std::string, std::unordered_set<EntityTypeID>> entityGroups;

		
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

		virtual bool canExecuteAction(Entity& entity, ActionType& actionType);
		virtual bool canExecuteAction(Player& player, ActionType& actionType);

		const Entity* getEntityAt(const Vector2f& pos) const;
		
		Entity* getEntity(int entityID)
		{
			auto iter = std::find_if(std::begin(entities), std::end(entities),
				[&](Entity const& p) { return p.id == entityID; });
			return iter == entities.end() ? nullptr : &*iter;
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

		const SGA::Parameter& getPlayerParameter(ParameterID id) const
		{
			auto it = playerParameterTypes->find(id);
			if (it != playerParameterTypes->end())
			{
				return it->second;
			}
			else
			{
				std::string s;
				s.append("Tried accessing unknown player parameter ID ");
				s.append(std::to_string(id));
				throw std::runtime_error(s);
			}
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
		
		const Parameter& getParameterType(int entityTypeID, int globalParameterID) const
		{
			const auto& entityType = getEntityType(entityTypeID);
			return entityType.parameters.find(globalParameterID)->second;
		}
		
		bool checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const
		{
			const auto& entityType = getEntityType(entityTypeID);
			for (const auto& parameter : entityType.parameters)
			{
				if (parameter.second.name == parameterName)
					return true;
			}
			
			return false;
		}
		
		int addPlayer(std::vector<int> actionIds)
		{
			auto& player = players.emplace_back(Player{nextPlayerID, 0, true});
			// Add parameters
			player.parameters.resize(playerParameterTypes->size());
			for(const auto& idParamPair : *playerParameterTypes)
			{
				player.parameters[idParamPair.second.index] = idParamPair.second.defaultValue;
			}

			// Add actions
			player.attachedActions.reserve(actionIds.size());
			for (auto actionTypeID : actionIds)
			{
				player.attachedActions.emplace_back(ActionInfo{ actionTypeID, 0 });
			}
			
			nextPlayerID++;
			return player.id;
		}

		int addEntity(const EntityType& type, int playerID, const Vector2f& position)
		{
			auto instance = type.instantiateEntity(nextEntityID);
			instance.ownerID = playerID;
			instance.position = position;
			entities.emplace_back(std::move(instance));

			nextEntityID++;
			return instance.id;
		}

		Entity* getEntity(Vector2f pos)
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
			Tile& targetTile = board.get(position.x, position.y);
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

		// Dirty trick to reuse code between const and non-const getter
		Player* getPlayer(int playerID) { return const_cast<Player*>(const_cast<const GameState*>(this)->getPlayer(playerID)); }
		
		const Player* getPlayer(int playerID) const
		{
			for(const auto& p : players)
			{
				if (p.id == playerID)
					return &p;
			}

			return nullptr;
		}

		std::vector<const Entity*> getPlayerEntities(int playerID) const
		{
			const auto* player = getPlayer(playerID);
			if (player == nullptr)
				return {};

			std::vector<const Entity*> ret;
			for(const auto& entity : entities)
			{
				if (entity.ownerID == playerID)
					ret.emplace_back(&entity);
			}

			return ret;
		}

		std::vector< Entity*> getPlayerEntities(int playerID)
		{
			const auto* player = getPlayer(playerID);
			if (player == nullptr)
				return {};

			std::vector<Entity*> ret;
			for ( auto& entity : entities)
			{
				if (entity.ownerID == playerID)
					ret.emplace_back(&entity);
			}

			return ret;
		}

		void applyFogOfWar(int playerID)
		{
			Grid2D<bool> visibilityMap(board.getWidth(), board.getHeight());
			for(const auto* entity : getPlayerEntities(playerID))
			{
				// Compute maximum sized rectangle around entity
				auto leftX = std::max<int>(0, entity->position.x - entity->lineOfSightRange);
				auto rightX = std::min<int>(board.getWidth() - 1, entity->position.x + entity->lineOfSightRange);
				auto leftY = std::max<int>(0, entity->position.y - entity->lineOfSightRange);
				auto rightY = std::min<int>(board.getHeight() - 1, entity->position.y + entity->lineOfSightRange);

				// Helper method for shadowcasting
				auto rayCallback = [&](const Vector2i& pos) -> bool
				{
					if (entity->position.distance(pos) > entity->lineOfSightRange)
					{
						return true;
					}
					
					visibilityMap[pos] = true;
					return board[pos].blocksSight;
				};
				
				// Shadowcasting
				for (int x = leftX; x <= rightX; x++)
				{
					visibilityMap.bresenhamRay(Vector2i(entity->position.x, entity->position.y), Vector2i{ x, leftY }, rayCallback);
					visibilityMap.bresenhamRay(Vector2i(entity->position.x, entity->position.y), Vector2i{ x, rightY }, rayCallback);
				}
				 
				 
				for (int y = leftY; y <= rightY; y++)
				{
					visibilityMap.bresenhamRay(Vector2i(entity->position.x, entity->position.y), Vector2i{ leftX, y}, rayCallback);
					visibilityMap.bresenhamRay(Vector2i(entity->position.x, entity->position.y), Vector2i{ rightX, y}, rayCallback);
				}
			}
			
			// Remove entities that are not visible
			auto it = entities.begin();
			while (it != entities.end())
			{
				if(!board.isInBounds((static_cast<int>(it->position.x), static_cast<int>(it->position.y)))||!visibilityMap.get(static_cast<int>(it->position.x), static_cast<int>(it->position.y)))
				{
					it = entities.erase(it);
				}
				else
				{
					++it;
				}
			}
			
			// Hide tiles that are not visible
			for (int y = 0; y < board.getHeight(); y++)
			{
				for (int x = 0; x < board.getWidth(); x++)
				{
					if (!visibilityMap.get(x, y))
					{
						auto& tile = board.get(x, y);
						tile = fogOfWarTile;
						tile.position = Vector2i(x, y);
					}
				}
			}

			fogOfWarId = playerID;
		}
	};
}