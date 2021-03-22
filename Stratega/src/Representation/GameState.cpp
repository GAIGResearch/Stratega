#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Condition.h>

namespace SGA
{
	GameState::GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes) :
		parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
		entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
		actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
		tileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes)),
		technologyTreeCollection(std::make_shared<TechnologyTreeCollection>()),
		currentPlayer(0),
		gameType(GameType::TBS),
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

	GameState::GameState()
		: parameterIDLookup(std::make_shared<std::unordered_map<std::string, ParameterID>>()),
		entityTypes(std::make_shared<std::unordered_map<int, EntityType>>()),
		actionTypes(std::make_shared<std::unordered_map<int, ActionType>>()),
		tileTypes(std::make_shared<std::unordered_map<int, TileType>>()),
		technologyTreeCollection(std::make_shared<TechnologyTreeCollection>()),
		currentPlayer(0),
		gameType(GameType::TBS),
		isGameOver(false),
		winnerPlayerID(-1),
		currentTick(1),
		tickLimit(-1),
		fogOfWarTile(-1, 0, 0),
		fogOfWarId(-1),
		board(0, 0, fogOfWarTile),
		nextEntityID(0),
		nextPlayerID(0)
	{
	}

	Entity* GameState::getEntity(int entityID)
	{
		auto iter = std::find_if(std::begin(entities), std::end(entities),
			[&](Entity const& p) { return p.id == entityID; });
		return iter == entities.end() ? nullptr : &*iter;
	}

	const Entity* GameState::getEntityConst(int entityID) const
	{
		auto iter = std::find_if(std::begin(entities), std::end(entities),
			[&](Entity const& p) { return p.id == entityID; });
		return iter == entities.end() ? nullptr : &*iter;

	}

	const EntityType& GameState::getEntityType(int entityTypeID) const
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

	const ActionType& GameState::getActionTypeConst(int actionTypeID)
	{
		auto it = actionTypes->find(actionTypeID);
		if (it != actionTypes->end())
		{
			return it->second;
		}
		else
		{
			std::string s;
			s.append("Tried accessing unknown action type with ID=");
			s.append(std::to_string(actionTypeID));
			throw std::runtime_error(s);
		}
	}

	int GameState::getParameterGlobalID(std::string parameterName)
	{
		int foundId = -1;
		auto  iterator = parameterIDLookup->find(parameterName);

		if (iterator != parameterIDLookup->end())
			foundId = iterator->second;

		return foundId;
	}

	int GameState::getActionTypeID(std::string parameterName)
	{
		int foundId = -1;
		for (const auto& element : *actionTypes)
		{
			if (element.second.name == parameterName)
				return element.second.id;
		}

		return foundId;
	}

	const SGA::Parameter& GameState::getPlayerParameter(ParameterID id) const
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

	Entity* GameState::getEntity(Vector2f pos, double maxDistance)
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

	const Parameter& GameState::getParameterType(int entityTypeID, int globalParameterID) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		return entityType.parameters.find(globalParameterID)->second;
	}

	bool GameState::checkEntityHaveParameter(int entityTypeID, const std::string& parameterName) const
	{
		const auto& entityType = getEntityType(entityTypeID);
		for (const auto& parameter : entityType.parameters)
		{
			if (parameter.second.name == parameterName)
				return true;
		}

		return false;
	}

	int GameState::addPlayer(std::vector<int> actionIds)
	{
		auto& player = players.emplace_back(Player{ nextPlayerID, 0, true });
		// Add parameters
		player.parameters.resize(playerParameterTypes->size());
		for (const auto& idParamPair : *playerParameterTypes)
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

	int GameState::addEntity(const EntityType& type, int playerID, const Vector2f& position)
	{
		auto instance = type.instantiateEntity(nextEntityID);
		instance.ownerID = playerID;
		instance.position = position;
		entities.emplace_back(std::move(instance));

		nextEntityID++;
		return instance.id;
	}

	Entity* GameState::getEntity(Vector2f pos)
	{
		for (auto& entity : entities)
		{
			if (entity.position == pos)
				return &entity;
		}

		return nullptr;
	}

	bool GameState::isWalkable(const Vector2i& position)
	{
		Tile& targetTile = board.get(position.x, position.y);
		Entity* targetUnit = getEntity(Vector2f(position));

		return targetUnit == nullptr && targetTile.isWalkable;
	}

	bool GameState::isInBounds(Vector2i pos)
	{
		return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
	}

	const ActionType& GameState::getActionType(int typeID) const
	{
		return actionTypes->find(typeID)->second;
	}

	bool GameState::isInBounds(Vector2f pos) const
	{
		return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
	}

	const Player* GameState::getPlayer(int playerID) const
	{
		for (const auto& p : players)
		{
			if (p.id == playerID)
				return &p;
		}

		return nullptr;
	}

	std::vector<const Entity*> GameState::getPlayerEntities(int playerID) const
	{
		const auto* player = getPlayer(playerID);
		if (player == nullptr)
			return {};

		std::vector<const Entity*> ret;
		for (const auto& entity : entities)
		{
			if (entity.ownerID == playerID)
				ret.emplace_back(&entity);
		}

		return ret;
	}

	std::vector<Entity*> GameState::getPlayerEntities(int playerID)
	{
		const auto* player = getPlayer(playerID);
		if (player == nullptr)
			return {};

		std::vector<Entity*> ret;
		for (auto& entity : entities)
		{
			if (entity.ownerID == playerID)
				ret.emplace_back(&entity);
		}

		return ret;
	}

	void GameState::applyFogOfWar(int playerID)
	{
		Grid2D<bool> visibilityMap(board.getWidth(), board.getHeight());
		for (const auto* entity : getPlayerEntities(playerID))
		{
			// Compute maximum sized rectangle around entity
			auto leftX = std::max<int>(0, static_cast<int>(entity->position.x - entity->lineOfSightRange));
			auto rightX = std::min<int>(static_cast<int>(board.getWidth() - 1), static_cast<int>(entity->position.x + entity->lineOfSightRange));
			auto leftY = std::max<int>(0, static_cast<int>(entity->position.y - entity->lineOfSightRange));
			auto rightY = std::min<int>(static_cast<int>(board.getHeight() - 1), static_cast<int>(entity->position.y + entity->lineOfSightRange));

			// Helper method for shadowcasting
			auto rayCallback = [&](const Vector2i& pos) -> bool
			{
				if (entity->position.distance(Vector2f(pos)) > entity->lineOfSightRange)
				{
					return true;
				}

				visibilityMap[pos] = true;
				return board[pos].blocksSight;
			};

			// Shadowcasting
			Vector2i pos(static_cast<int>(entity->position.x), static_cast<int>(entity->position.y));
			for (int x = leftX; x <= rightX; x++)
			{
				visibilityMap.bresenhamRay(pos, Vector2i{ x, leftY }, rayCallback);
				visibilityMap.bresenhamRay(pos, Vector2i{ x, rightY }, rayCallback);
			}


			for (int y = leftY; y <= rightY; y++)
			{
				visibilityMap.bresenhamRay(pos, Vector2i{ leftX, y }, rayCallback);
				visibilityMap.bresenhamRay(pos, Vector2i{ rightX, y }, rayCallback);
			}
		}

		// Remove entities that are not visible
		auto it = entities.begin();
		while (it != entities.end())
		{
			if (!board.isInBounds(static_cast<int>(it->position.x), static_cast<int>(it->position.y)) || !visibilityMap.get(static_cast<int>(it->position.x), static_cast<int>(it->position.y)))
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
	
	bool GameState::canExecuteAction(Entity& entity, const ActionType& actionType)
	{
		//Check preconditions
		for (const auto& precondition : actionType.preconditions)
		{
			if (!precondition->isFullfilled(*this, { ActionTarget::createEntityActionTarget(entity.id) }))
			{
				return false;
			}
		}

		return true;
	}

	bool GameState::canExecuteAction(Player& player, const ActionType& actionType)
	{
		//Check preconditions
		for (const auto& precondition : actionType.preconditions)
		{
			if (!precondition->isFullfilled(*this, { ActionTarget::createPlayerActionTarget(player.id) }))
			{
				return false;
			}
		}

		return true;
	}

	const Entity* GameState::getEntityAt(const Vector2f& pos) const
	{
		for(const auto& entity : entities)
		{
			if(static_cast<int>(pos.x) == static_cast<int>(entity.position.x) && static_cast<int>(pos.y) == static_cast<int>(entity.position.y))
			{
				return &entity;
			}
		}

		return nullptr;
	}
}
