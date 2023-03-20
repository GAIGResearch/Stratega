#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Condition.h>
#include <vector>
#include <algorithm>
#pragma warning(disable: 5045)
#include<Stratega/Utils/warnings.h>

DISABLE_WARNING_PUSH
#if defined(__clang__)    
	DISABLE_WARNING_NULL_DEREFERENCE
#elif defined(__GNUC__)
	DISABLE_WARNING_NULL_DEREFERENCE
#endif

namespace SGA
{
	GameState::GameState(Grid2D<Tile>&& newBoard, const std::unordered_map<int, TileType>& /*tileTypes*/) :
		gameOver(false),				
		winnerPlayerID(-1),
		currentTick(1),
		tickLimit(-1),				
		gameType(GameType::TBS),
		fogOfWarTile(nullptr, 0, 0),
		board(std::move(newBoard)),
		players(),		
		fogOfWarId(-1),
		currentPlayer(0),
		fogOfWarApplied(false)
		
	{
	}

	GameState::GameState() :
		gameOver(false),				
		winnerPlayerID(-1),
		currentTick(1),
		tickLimit(-1),		
		gameType(GameType::TBS),
		fogOfWarTile(nullptr, 0, 0),
		board(0, 0, fogOfWarTile),		
		fogOfWarId(-1),
		currentPlayer(0),
		fogOfWarApplied(false)		
	{		
	}

	Entity* GameState::getEntity(int entityID)
	{
		auto iter = std::find_if(std::begin(entities), std::end(entities),
			[&](Entity const& p) { return p.getID() == entityID; });
		return iter == entities.end() ? nullptr : &*iter;
	}

	const Entity* GameState::getEntityConst(int entityID) const
	{
		auto iter = std::find_if(std::begin(entities), std::end(entities),
			[&](Entity const& p) { return p.getID() == entityID; });
		return iter == entities.end() ? nullptr : &*iter;

	}

	int GameState::addPlayer(Player& p)
	{
		int playerID = static_cast<int>(players.size());
		players.emplace_back(p);
		return playerID;
	}

	int GameState::addEntity(const EntityType& type, int playerID, const Vector2f& position)
	{
		auto instance = type.instantiateEntity(nextEntityID);
		instance.setOwnerID(playerID);
		instance.setPosition(position);
		entities.emplace_back(std::move(instance));
		nextEntityID++;

		return instance.getID();
	}

	Entity* GameState::getEntity(Vector2f pos, float maxDistance)
	{
		for (auto& entity : entities)
		{
			if (entity.getPosition() == pos)
				return &entity;
			else if (maxDistance > 0.0 && (entity.getPosition().distance(pos) <= maxDistance))
				return &entity;
		}

		return nullptr;
	}


	const Player* GameState::getPlayer(int playerID) const
	{
		for (const auto& p : players)
		{
			if (p.getID() == playerID)
				return &p;
		}

		return nullptr;
	}

	
	std::vector<int> GameState::whoCanPlay() const
	{
		std::vector<int> playerIDs;
		if (currentPlayer != -1)
		{
			playerIDs.emplace_back(currentPlayer);
		}
		else
		{
			for (const auto& p : players)
			{
				playerIDs.emplace_back(p.getID());
			}
		}		
		return playerIDs;
	}
	

	bool GameState::canPlay(int playerID) const
	{
		return (currentPlayer == playerID || currentPlayer == -1);
	}
	
	
	std::vector<Entity> GameState::getPlayerEntities(int playerID, EntityCategory entityCategory) const
	{
		auto* player = getPlayer(playerID);
		if (player == nullptr)
			throw std::runtime_error("No player associated to ID " + std::to_string(playerID));

		std::vector<Entity> ret;
		for (auto& entity : entities)
		{
			if (entity.getOwnerID() == playerID)
			{
				//Either no category was especified (default argment) or the entity type id belongs to this category.
				if (entityCategory == EntityCategory::Null || gameInfo->getGameDescription().isFromCategory(entityCategory, entity.getEntityTypeID()))
				{
					ret.emplace_back(entity);
				}
			}
		}
		return ret;
	}

	std::vector<Entity> GameState::getNonPlayerEntities(int playerID, EntityCategory entityCategory) const
	{
		std::vector<Entity> ret;
		for (auto& entity : entities)
		{
			if (entity.getOwnerID() != playerID)
			{
				//Either no category was especified (default argment) or the entity type id belongs to this category.
				if (entityCategory == EntityCategory::Null || gameInfo->getGameDescription().isFromCategory(entityCategory, entity.getEntityTypeID()))
				{
					ret.emplace_back(entity);
				}
			}
		}

		return ret;
	}


	int GameState::getPlayerScore(int playerID) const
	{
		if (hasPlayerParameter("Score"))
			return static_cast<int>(getPlayerParameter(playerID, "Score"));
		return 0; 
	}

	double GameState::getPlayerParameter(int playerID, const std::string& paramName) const
	{
		const Player* p = getPlayer(playerID);
		if (p != nullptr)
		{
			const auto parameterTypes = gameInfo->getPlayerParameterTypes();
			for (const auto& param : parameterTypes)
			{
				if (param.second.getName() == paramName)
				{
					return p->getRawParameterAt(param.second.getIndex());
				}
			}
			throw std::runtime_error("No parameter " + paramName + " associated to player ID " + std::to_string(playerID));

		}else throw std::runtime_error("No player associated to ID " + std::to_string(playerID));
	}

	bool GameState::hasPlayerParameter(const std::string& paramName) const
	{
		const auto parameterTypes = gameInfo->getPlayerParameterTypes();
		for (const auto& param : parameterTypes)
		{
			if (param.second.getName() == paramName)
			{
				return true;
			}
		}
		return false;
	}

	//NOTE: For the moment, all players have the same parameters (hence playerID is not used).
	std::vector<std::string> GameState::getPlayerParameterNames(int /*playerID*/) const
	{
		std::vector<std::string> paramNames;
		const auto parameterTypes = gameInfo->getPlayerParameterTypes();
		for (const auto& param : parameterTypes)
			paramNames.emplace_back(param.second.getName());
		
		return paramNames;
	}

	std::unordered_map<std::string, double> GameState::getPlayerParameters(int playerID) const
	{
		const Player* p = getPlayer(playerID);
		if (p == nullptr)
			throw std::runtime_error("No player associated to ID " + std::to_string(playerID));

		std::unordered_map<std::string, double> params;
		const auto parameterTypes = gameInfo->getPlayerParameterTypes();
		for (const auto& param : parameterTypes)
			params.emplace(param.second.getName(), p->getRawParameterAt(param.second.getIndex()));
		
		return params;
	}


	void GameState::applyFogOfWar(int playerID)
	{
		Grid2D<bool> visibilityMap(board.getWidth(), board.getHeight());
		for (auto entity : entities)
		{
			if(entity.getOwnerID() == playerID)
			{
				// Compute maximum sized rectangle around entity
				auto leftX = std::max<int>(0, static_cast<int>(entity.x() - entity.getLineOfSightRange()));
				auto rightX = std::min<int>(static_cast<int>(board.getWidth() - 1), static_cast<int>(entity.x() + entity.getLineOfSightRange()));
				auto leftY = std::max<int>(0, static_cast<int>(entity.y() - entity.getLineOfSightRange()));
				auto rightY = std::min<int>(static_cast<int>(board.getHeight() - 1), static_cast<int>(entity.y() + entity.getLineOfSightRange()));

				// Helper method for shadowcasting
				auto rayCallback = [&](const Vector2i& pos) -> bool
				{
					if (entity.getPosition().distance(Vector2f(pos)) > entity.getLineOfSightRange())
					{
						return true;
					}

					visibilityMap[pos] = true;
					return board[pos].blocksSight();
				};

				// Shadowcasting
				Vector2i pos(static_cast<int>(entity.x()), static_cast<int>(entity.y()));
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
		}

		// Remove entities that are not visible
		auto it = entities.begin();
		while (it != entities.end())
		{
			if (!board.isInBounds(static_cast<int>(it->x()), static_cast<int>(it->y())) || 
				!visibilityMap.get(static_cast<int>(it->x()), static_cast<int>(it->y())))
			{
				it = entities.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Hide tiles that are not visible
		for (int y = 0; y < static_cast<int>(board.getHeight()); y++)
		{
			for (int x = 0; x < static_cast<int>(board.getWidth()); x++)
			{
				if (!visibilityMap.get(x, y))
				{
					auto& tile = board.get(x, y);
					tile = fogOfWarTile;
					tile.setPosition(x, y);
				}
			}
		}

		fogOfWarId = playerID;
	}
	
	bool GameState::canExecuteAction(const Entity& entity, const ActionType& actionType) const
	{
		//Check preconditions
		for (const auto& precondition : actionType.getPreconditions())
		{
			if (!precondition->isFullfiled(*this, { ActionTarget::createEntityActionTarget(entity.getID()) }))
			{
				return false;
			}
		}

		return true;
	}

	bool GameState::canExecuteAction(const Player& player, const ActionType& actionType) const
	{
		//Check preconditions
		for (const auto& precondition : actionType.getPreconditions())
		{
			if (!precondition->isFullfiled(*this, { ActionTarget::createPlayerActionTarget(player.getID()) }))
			{
				return false;
			}
		}

		return true;
	}

	std::vector<ActionType> GameState::getPlayerActionTypes(int playerID) const
	{
		const Player* p = getPlayer(playerID);
		if(p == nullptr)
			throw std::runtime_error("No player associated to ID " + std::to_string(playerID));

		std::vector<ActionType> aTypes;
		for (const ActionInfo aInfo : p->getAttachedActions())
		{
			ActionType at = gameInfo->getActionType(aInfo.actionTypeID);
			aTypes.emplace_back(at);
		}
		return aTypes;
	}

	const Entity* GameState::getEntityAt(const Vector2f& pos) const
	{
		for(const auto& entity : entities)
		{
			if (static_cast<int>(pos.x) == static_cast<int>(entity.x()) && static_cast<int>(pos.y) == static_cast<int>(entity.y()))
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
		const TechnologyTreeNode& technologyNode = gameInfo->getTechnologyTreeCollection().getTechnology(technologyID);

		const std::vector<int>& parentsIDs = technologyNode.parentIDs;

		for (auto& parent : parentsIDs)
		{
			const TechnologyTreeNode& technologyParentNode = gameInfo->getTechnologyTreeCollection().getTechnology(parent);

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

	bool GameState::isResearched(int playerID, const std::string& technologyName) {
		// this function throw an error if the technologyName is incorrect
		int techID = gameInfo->getTechnologyTypeID(technologyName);
		return isResearched(playerID, techID);
	}

	void GameState::researchTechnology(int playerID, int technologyID)
	{
		//Get researched technologies of player
		const auto& researchedPairList = researchedTechnologies.find(playerID);

		//Find technology index and add it to the researched list			
		researchedPairList->second.emplace_back(technologyID);
	}

	void GameState::initResearchTechs()
	{
		for(int i = 0; i < static_cast<int>(players.size()); ++i)
			researchedTechnologies[i] = {};
	}


	/* BOARD */

	bool GameState::isWalkable(const Vector2i& position)
	{
		Tile& targetTile = board.get(position.x, position.y);
		Entity* targetUnit = getEntity(Vector2f(position));

		return targetUnit == nullptr && targetTile.isWalkable();
	}

	bool GameState::isInBounds(const Vector2i& pos) const
	{
		return pos.x >= 0 && pos.x < static_cast<int>(board.getWidth()) && pos.y >= 0 && pos.y < static_cast<int>(board.getHeight());
	}

	bool GameState::isInBounds(const Vector2f& pos) const
	{
		return pos.x >= 0 && pos.x < static_cast<double>(board.getWidth()) && pos.y >= 0 && pos.y < static_cast<double>(board.getHeight());
	}

	void GameState::initBoard(int boardWidth, std::vector<Tile>& tiles)
	{
		board = Grid2D<Tile>(boardWidth, tiles.begin(), tiles.end());
	}


	const Tile& GameState::getTileAt(const Vector2i& pos) const
	{
		if(isInBounds(pos))
			return board[pos];
		throw std::runtime_error("Access to board out of bounds: " + std::to_string(pos.x) + "," + std::to_string(pos.y));
	}

	const Tile& GameState::getTileAt(int x, int y) const
	{
		return getTileAt({x,y});
	}


	/* PRINTS */


	void GameState::printStateInfo() const
	{
		std::cout << "---------[StateInfo]---------" << std::endl;
		std::cout << "Number of entities: " << entities.size() << std::endl;

		//Print entities
		for (auto& entity : entities)
		{
			std::cout << "[OwnerID]" << entity.getOwnerID() << std::endl;			
			std::cout << "	[type]: " << gameInfo->getEntityType(entity.getEntityTypeID()).getName() << " [entityID]: "<< entity.getID() << std::endl;
		}
	}

	void GameState::printBoard() const
	{
		std::string map;
		std::cout << "---------[Board]---------" << std::endl;
		//Add tiles
		for (size_t y = 0; y < board.getHeight(); ++y)
		{
			for (size_t x = 0; x < board.getWidth(); ++x)
			{
				//Get tile type
				map += gameInfo->getTileType(board.get(static_cast<int>(x), static_cast<int>(y)).getTileTypeID()).getSymbol();
				map += "  ";
			}
			map += "\n";
		}

		//Add entities
		for (auto& entity : entities)
		{
			auto& pos = entity.getPosition();
			const char symbol = gameInfo->getEntityType(entity.getEntityTypeID()).getSymbol();
			const char ownerID = std::to_string(entity.getOwnerID())[0];
			const int entityMapIndex = static_cast<int>((pos.y * static_cast<double>(board.getWidth()) + pos.x) * 3 + pos.y);

			map[entityMapIndex] = symbol;

			if (!entity.isNeutral())
				map[static_cast<int>(static_cast<int>(entityMapIndex) + 1)] = ownerID;
		}
		//Print map
		std::cout << map;
	}

	void GameState::printBoard(int playerID) const
	{
		if(static_cast<size_t>(playerID) < players.size())
		{
			GameState stateWithFOG = *this;
			
			if(fogOfWarApplied)
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

	void GameState::printActionInfo(const Action& action) const
	{
		std::cout << "ActionInfo";
		if(action.getActionFlag() == ActionFlag::AbortContinuousAction)
		{
			std::cout << "[AbortContinuousAction]" << std::endl;
		}
		else if (action.getActionFlag() == ActionFlag::EndTickAction)
		{
			std::cout << "[EndTick]"  << std::endl;
		}
		else
		{
			const ActionType& actionType = action.getActionType();
			if(actionType.getName() == "Move"){
				std::cout << "[" << actionType.getName() << "],";
				int entityID = action.getSourceID();
				auto& entityType = getEntityConst(entityID)->getEntityType();
				auto pos = getEntityConst(entityID)->getPosition();

				std::cout << " [SourceType Entity: " << entityType.getName() << " " << entityID << " (" << pos.x <<", "<< pos.y << ")],";
				std::cout << " [ActionTargets";
				for (size_t i = 0; i < actionType.getTargets().size(); i++)
				{
					std::cout << "(Target: " << actionType.getTargets()[i].first.getTypeString() << ", " << action.getTargets()[i + 1].getValueString(*this) << ")";
				}
				std::cout << "]" << std::endl;

				return;
			}

			std::cout << "["<< actionType.getName() <<"]," ;

			//Print source
			if (actionType.getSourceType() == ActionSourceType::Player)
				std::cout << " [SourceType Player: " << action.getOwnerID() << "],";
			else
			{				
				int entityID = action.getSourceID();
				auto& entityType = getEntityConst(entityID)->getEntityType();
				
				std::cout << " [SourceType Entity: "<<entityType.getName() <<" "<<entityID<<"],";
			}

			std::cout << " [ActionTargets" ;
			for (size_t i = 0; i < actionType.getTargets().size(); i++)
			{
				std::cout << "(Target: "<<actionType.getTargets()[i].first.getTypeString()<<", " << action.getTargets()[i + 1].getValueString(*this) << ")";
			}
			std::cout << "]"<<std::endl;
		}
		
	}
}
