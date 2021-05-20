#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Condition.h>

namespace SGA
{
	GameState::GameState(Grid2D<Tile>&& board, const std::unordered_map<int, TileType>& tileTypes) :
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

	GameState::GameState() :
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

	Entity* GameState::getEntity(Vector2f pos, float maxDistance)
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

	int GameState::addPlayer(std::vector<int> actionIds)
	{
		auto& player = players.emplace_back(Player{ nextPlayerID, 0, true });
		// Add parameters
		player.parameters.resize(gameInfo->playerParameterTypes->size());
		for (const auto& idParamPair : *gameInfo->playerParameterTypes)
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

	bool GameState::isInBounds(Vector2i pos) const
	{
		return pos.x >= 0 && pos.x < board.getWidth() && pos.y >= 0 && pos.y < board.getHeight();
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


	std::vector<Entity*> GameState::getPlayerEntities(int playerID, EntityCategory entityCategory)
	{
		const auto* player = getPlayer(playerID);
		if (player == nullptr)
			return {};

		std::vector<Entity*> ret;
		for (auto& entity : entities)
		{
			if (entity.ownerID == playerID)
			{
				//Either no category was especified (default argment) or the entity type id belongs to this category.
				if (entityCategory == EntityCategory::Null || this->gameInfo->gameDescription->isFromCategory(entityCategory, entity.typeID))
				{
					ret.emplace_back(&entity);
				}
			}
		}
		return ret;
	}

	std::vector<Entity*> GameState::getNonPlayerEntities(int playerID, EntityCategory entityCategory)
	{
		std::vector<Entity*> ret;
		for (auto& entity : entities)
		{
			if (entity.ownerID != playerID)
			{
				//Either no category was especified (default argment) or the entity type id belongs to this category.
				if (entityCategory == EntityCategory::Null || this->gameInfo->gameDescription->isFromCategory(entityCategory, entity.typeID))
				{
					ret.emplace_back(&entity);
				}
			}
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
	
	bool GameState::canExecuteAction(const Entity& entity, const ActionType& actionType) const
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

	bool GameState::canExecuteAction(const Player& player, const ActionType& actionType) const
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

	void GameState::printStateInfo() const
	{
		std::cout << "---------[StateInfo]---------" << std::endl;
		std::cout << "Number of entities: " << entities.size() << std::endl;

		//Print entities
		for (auto& entity : entities)
		{
			std::cout << "[OwnerID]" <<entity.ownerID << std::endl;			
			std::cout << "	[type]: " << gameInfo->getEntityType(entity.typeID).name << " [entityID]: "<<entity.id<< std::endl;
		}
	}

	void GameState::printBoard() const
	{
		std::string map;
		std::cout << "---------[Board]---------" << std::endl;
		//Add tiles
		for (int y = 0; y < board.getHeight(); ++y)
		{
			for (int x = 0; x < board.getWidth(); ++x)
			{
				//Get tile type
				map += gameInfo->getTileType(board.get(x, y).tileTypeID).symbol;
				map += "  ";
			}
			map += "\n";
		}

		//Add entities
		for (auto& entity : entities)
		{
			auto& pos = entity.position;
			const char symbol = gameInfo->getEntityType(entity.typeID).symbol;
			const char ownerID = std::to_string(entity.ownerID)[0];
			const int entityMapIndex = (pos.y * board.getWidth() + pos.x) * 3 + pos.y;

			map[entityMapIndex] = symbol;

			if (!entity.isNeutral())
				map[entityMapIndex + 1] = ownerID;
		}
		//Print map
		std::cout << map;
	}

	void GameState::printBoard(int playerID) const
	{
		if(playerID < players.size())
		{
			GameState stateWithFOG = *this;
			stateWithFOG.applyFogOfWar(playerID);
			stateWithFOG.printBoard();
		}
		else
		{
			std::cout << "Player not found" << std::endl;
		}		
	}
	
	void GameState::printEntityInfo(int entityID)
	{
		std::cout << "EntityInfo";
		const auto* entity = getEntity(entityID);

		if (entity)
		{
			std::cout << "[" << gameInfo->getEntityType(entity->typeID).name << "],";
			std::cout << " [ID: " <<entity->id<<"],";
			std::cout << " [OwnerID: " <<entity->ownerID<<"],";
			std::cout << " [Position: "<< entity->position.x << "," << entity->position.y <<"]";

			

			int parameterID = 0;
			auto& entityType = gameInfo->getEntityType(entity->typeID);
			if (entity->parameters.empty())
			{
				std::cout << std::endl;
				return;
			}				
				
			std::cout << ", [Parameters: ";
			for (auto& parameter : entity->parameters)
			{
				std::cout << "(" << entityType.parameters.find(parameterID++)->second.name <<": "<< parameter<<")";
			}

			std::cout << "]"<<std::endl;
		}
		else
		{
			std::cout << "Entity not found" << std::endl;
		}
	}

	void GameState::printActionInfo(Action& action) const
	{
		std::cout << "ActionInfo";
		if(action.actionTypeFlags== ActionFlag::AbortContinuousAction)
		{
			std::cout << "[AbortContinuousAction]" << std::endl;
		}
		else if (action.actionTypeFlags == ActionFlag::EndTickAction)
		{
			std::cout << "[EndTick]"  << std::endl;
		}
		else
		{
			const ActionType& actionType = gameInfo->getActionType(action.actionTypeID);
			std::cout << "["<< actionType.name <<"]," ;
			
			//Print source
			if (actionType.sourceType == ActionSourceType::Player)
				std::cout << " [SourceType Player: " << action.ownerID << "],";
			else
			{
				int entityID = action.getSourceID();
				int entityTypeID = getEntityConst(entityID)->typeID;
				auto& entityType = gameInfo->getEntityType(entityTypeID);
				
				std::cout << " [SourceType Entity: "<<entityType.name<<" "<<entityID<<"],";
			}
				



			std::cout << " [ActionTargets" ;
			for (size_t i = 0; i < actionType.actionTargets.size(); i++)
			{
				std::cout << "(Target: "<<actionType.actionTargets[i].first.getTypeString()<<", " << action.targets[i + 1].getValueString(*this) << ")";
			}
			std::cout << "]"<<std::endl;
		}
		
	}
}
