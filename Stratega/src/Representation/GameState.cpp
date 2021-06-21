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
		fogOfWarTile(-1, nullptr, 0, 0),
		fogOfWarId(-1),
		board(std::move(board)),
		players()
	{
	}

	GameState::GameState() :
		currentPlayer(0),
		gameType(GameType::TBS),
		isGameOver(false),
		winnerPlayerID(-1),
		currentTick(1),
		tickLimit(-1),
		fogOfWarTile(-1, nullptr, 0, 0),
		fogOfWarId(-1),
		board(0, 0, fogOfWarTile)
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

	int GameState::addPlayer(Player p)
	{
		int playerID = players.size();
		players.emplace_back(p);
		return playerID;
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

	Entity* GameState::getEntity(Vector2f pos, float maxDistance)
	{
		for (auto& entity : entities)
		{
			if (entity.position == pos)
				return &entity;
			else if (maxDistance > 0.0 && (entity.position.distance(pos) <= maxDistance))
				return &entity;
		}

		return nullptr;
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
				if (entityCategory == EntityCategory::Null || this->gameInfo->gameDescription->isFromCategory(entityCategory, entity.getEntityTypeID()))
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
				if (entityCategory == EntityCategory::Null || this->gameInfo->gameDescription->isFromCategory(entityCategory, entity.getEntityTypeID()))
				{
					ret.emplace_back(&entity);
				}
			}
		}

		return ret;
	}


	int GameState::getPlayerScore(int playerID) const
	{
		if (hasPlayerParameter("Score"))
			return getPlayerParameter(playerID, "Score");
		return 0; 
	}

	double GameState::getPlayerParameter(int playerID, std::string paramName) const
	{
		const Player* p = getPlayer(playerID);
		if (p != nullptr)
		{
			for (const auto& param : *gameInfo->playerParameterTypes)
			{
				if (param.second.name == paramName)
				{
					return p->parameters[param.second.index];
				}
			}
			throw std::runtime_error("No parameter " + paramName + " associated to player ID " + std::to_string(playerID));

		}else throw std::runtime_error("No player associated to ID " + std::to_string(playerID));
	}

	bool GameState::hasPlayerParameter(std::string paramName) const
	{
		for (const auto& param : *gameInfo->playerParameterTypes)
		{
			if (param.second.name == paramName)
			{
				return true;
			}
		}
		return false;
	}

	//NOTE: For the moment, all players have the same parameters (hence playerID is not used).
	std::vector<std::string> GameState::getPlayerParameterNames(int playerID) const
	{
		std::vector<std::string> paramNames;
		for (const auto& param : *gameInfo->playerParameterTypes)
			paramNames.emplace_back(param.second.name);
		
		return paramNames;
	}

	std::unordered_map<std::string, double> GameState::getPlayerParameters(int playerID) const
	{
		const Player* p = getPlayer(playerID);
		std::unordered_map<std::string, double> params;
		if (p != nullptr) for (const auto& param : *gameInfo->playerParameterTypes)
			params.emplace(param.second.name, p->parameters[param.second.index]);
		else throw std::runtime_error("WARNING: No player associated to ID " + std::to_string(playerID));

		return params;
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
			if (!precondition->isFulfilled(*this, { ActionTarget::createEntityActionTarget(entity.id) }))
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
			if (!precondition->isFulfilled(*this, { ActionTarget::createPlayerActionTarget(player.id) }))
			{
				return false;
			}
		}

		return true;
	}

	const std::vector<ActionType> GameState::getPlayerActionTypes(int playerID) const
	{
		const Player* p = getPlayer(playerID);
		std::vector<ActionType> aTypes;
		if (p != nullptr) for (const ActionInfo aInfo : p->attachedActions)
		{
			ActionType at = gameInfo->getActionType(aInfo.actionTypeID);
			aTypes.emplace_back(at);
		}
		else throw std::runtime_error("WARNING: No player associated to ID " + std::to_string(playerID));
		return aTypes;
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

	/* TECHNOLOGIES */

	bool GameState::canResearch(int playerID, int technologyID) const
	{
		//Check if is researched
		if (isResearched(playerID, technologyID))
			return false;

		//Check if technology parents are researched		
		const TechnologyTreeNode& technologyNode = gameInfo->technologyTreeCollection->getTechnology(technologyID);

		const std::vector<int>& parentsIDs = technologyNode.parentIDs;

		for (auto& parent : parentsIDs)
		{
			const TechnologyTreeNode& technologyParentNode = gameInfo->technologyTreeCollection->getTechnology(parent);

			if (!isResearched(playerID, technologyParentNode.id))
			{
				return false;
			}
		}
		return true;
	}

	bool GameState::isResearched(int playerID, int technologyID) const
	{
		//Search if the technology is found in the list of researchedtechnologies
		const auto& researchedPairList = researchedTechnologies.find(playerID);

		for (auto& element : researchedPairList->second)
		{
			if (element == technologyID)
				return true;
		}
		return false;
	}

	void GameState::researchTechnology(int playerID, int technologyID)
	{
		//Get researched technologies of player
		const auto& researchedPairList = researchedTechnologies.find(playerID);

		//Find technology index and add it to the researched list			
		researchedPairList->second.emplace_back(technologyID);
	}

	/* BOARD */

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

	/* PRINTS */


	void GameState::printStateInfo() const
	{
		std::cout << "---------[StateInfo]---------" << std::endl;
		std::cout << "Number of entities: " << entities.size() << std::endl;

		//Print entities
		for (auto& entity : entities)
		{
			std::cout << "[OwnerID]" <<entity.ownerID << std::endl;			
			std::cout << "	[type]: " << gameInfo->getEntityType(entity.getEntityTypeID()).name << " [entityID]: "<<entity.id<< std::endl;
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
				map += gameInfo->getTileType(board.get(x, y).getTileTypeID()).symbol;
				map += "  ";
			}
			map += "\n";
		}

		//Add entities
		for (auto& entity : entities)
		{
			auto& pos = entity.position;
			const char symbol = gameInfo->getEntityType(entity.getEntityTypeID()).symbol;
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
	
	void GameState::printEntityInfo(int entityID) const
	{
		const auto* entity = getEntityConst(entityID);
		if (entity)  	
			entity->printInfo();
		else 			
			std::cout << "Entity not found" << std::endl;
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
			const ActionType& actionType = action.getActionType();
			std::cout << "["<< actionType.name <<"]," ;
			
			//Print source
			if (actionType.sourceType == ActionSourceType::Player)
				std::cout << " [SourceType Player: " << action.ownerID << "],";
			else
			{				
				int entityID = action.getSourceID();
				auto& entityType = getEntityConst(entityID)->getEntityType();
				
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
