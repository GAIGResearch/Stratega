#include <Stratega/ForwardModel/ActionType.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameState.h>


namespace SGA
{
	ActionTarget ActionTarget::createPositionActionTarget(Vector2f position)
	{
		return ActionTarget(Position, Data{position });
	}

	ActionTarget ActionTarget::createEntityActionTarget(int entityID)
	{
		return ActionTarget(EntityReference, Data{ EntityReference, entityID });
	}

	ActionTarget ActionTarget::createEntityTypeActionTarget(EntityTypeID entityTypeID)
	{
		return ActionTarget(EntityTypeReference, Data{ EntityTypeReference, entityTypeID });
	}

	ActionTarget ActionTarget::createTileTypeActionTarget(EntityTypeID tileTypeID)
	{
		return ActionTarget(TileTypeReference, Data{ TileTypeReference, tileTypeID });
	}
	
	ActionTarget ActionTarget::createPlayerActionTarget(int playerID)
	{
		return ActionTarget(Type::PlayerReference, Data{ PlayerReference, playerID });
	}
	
	
	ActionTarget ActionTarget::createGameStateActionTarget()
	{
		return ActionTarget(Type::Gamestate, Data{});
	}

	ActionTarget ActionTarget::createTechnologyEntityActionTarget(int technologyID)
	{
		return ActionTarget(TechnologyReference, Data{ TechnologyReference, technologyID });
	}

	ActionTarget ActionTarget::createContinuousActionActionTarget(int continuousActionID)
	{
		return ActionTarget(Type::ContinuousActionReference, Data{ ContinuousActionReference, continuousActionID });
	}

	int ActionTarget::getPlayerID() const
	{
		if (targetType == PlayerReference)
		{
			return data.playerID;
		}

		throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
	}

	int ActionTarget::getPlayerID(const GameState& state) const
	{
		if (targetType == PlayerReference)
		{
			return data.playerID;
		}
		else if(targetType == EntityReference)
		{
			return state.getEntityConst(data.entityID)->getOwnerID();
		}

		throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
	}
	
	const std::unordered_set<EntityTypeID>& ActionTarget::getSpawnableEntities(const GameState& state) const
	{
		if (targetType == PlayerReference)
		{
			return state.getGameInfo()->getPlayerSpawnableTypes();
		}
		else if (targetType == EntityReference)
		{
			return getEntityConst(state).getEntityType().getSpawnableEntityTypes();
		}

		throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
	}

	Vector2f ActionTarget::getPosition() const
	{
		if (targetType == Position)
		{
			return data.position;
		}
		else {
			throw std::runtime_error("Cannot call getPosition when targetType != Position");
		}
	}

	Vector2f ActionTarget::getPosition(const GameState& state) const
	{
		if (targetType == Position)
		{
			return data.position;
		}
		else if(targetType == EntityReference)
		{
			return getEntity(const_cast<GameState&>(state))->getPosition();
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	const Entity& ActionTarget::getEntityConst(const GameState& state) const
	{
		if (targetType == Type::EntityReference)
		{
			return *state.getEntityConst(data.entityID);
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	Entity* ActionTarget::getEntity(GameState& state) const
	{
		if (targetType == Type::EntityReference)
		{
			auto* entity = state.getEntity(data.entityID);
		
			return entity;
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	Player& ActionTarget::getPlayer(GameState& state) const
	{
		if (targetType == Type::PlayerReference)
		{
			auto* player = state.getPlayer(data.playerID);
			if (player == nullptr)
			{
				throw std::runtime_error("An action-target contained an not existing player with id " + std::to_string(data.playerID));
			}
			return *player;
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	const Player& ActionTarget::getPlayerConst(const GameState& state) const
	{
		if (targetType == Type::PlayerReference)
		{
			auto* player = state.getPlayer(data.playerID);
			if (player == nullptr)
			{
				throw std::runtime_error("An action-target contained an not existing player with id " + std::to_string(data.playerID));
			}
			return *player;
		}

		throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
	}

	const EntityType& ActionTarget::getEntityType(const GameState& state) const
	{
		if (targetType == EntityTypeReference)
		{
			const auto& type = state.getGameInfo()->getEntityType(data.entityTypeID);
			return type;
		}
		else if(targetType == EntityReference)
		{
			const auto& type = state.getEntityConst(data.entityID)->getEntityType();
			return type;
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	const TileType& ActionTarget::getTileType(const GameState& state) const
	{
		if (targetType == TileTypeReference)
		{
			const auto& type = state.getGameInfo()->getTileType(data.entityTypeID);
			return type;
		}
		else
		{
			throw std::runtime_error("Target type " + std::to_string(int(targetType)) + " not recognised in action target.");
		}
	}

	bool ActionTarget::isValid(const GameState& state) const
	{
		switch (targetType)
		{
		case EntityReference:
			return state.getEntityConst(data.entityID);
			break;
		default: return true;
		}
	}
		
	bool ActionTarget::isValidWithTargets(const GameState& state,const  ActionType& actionType, const std::vector<ActionTarget>& actionTargets)
	{
		auto isValid = true;
		size_t i = 0;
		for(auto& actionTarget : actionType.getTargets())
		{
			//Check valid targets
			if (!actionTarget.first.isValid(state, actionTargets[i + 1], actionTargets[0]))
				return false;
			
			
			for (const auto& condition : actionTarget.second)
			{
				if (!condition->isFullfiled(state, actionTargets))
					isValid = false;
			}
			i++;
		}
		
		return isValid;
	}

	std::string ActionTarget::getValueString(const GameState& state) const
	{
		switch (targetType)
		{
		case EntityReference:
			
			return getEntityType(state).getName() +" "+std::to_string(getEntityID());
			break;
		case Position:
		{
			const auto position = getPosition(state);
				
			std::string posString;
			std::string num_text = std::to_string(position.x);
			std::string rounded = num_text.substr(0, num_text.find(".") + 3);
			posString += num_text.substr(0, num_text.find(".") + 3);
			posString += ",";
			num_text = std::to_string(position.y);
			posString += num_text.substr(0, num_text.find(".") + 3);
			return posString;
		}
		break;
		case PlayerReference:
			return std::to_string(getPlayerID());
			break;
		case EntityTypeReference:
			{
			const auto& entityType = getEntityType(state);
			return entityType.getName();
			}			
			break;
		case TechnologyReference:
			{
			int technologyID = getTechnologyID();
			std::string technologyName = state.getGameInfo()->getTechnologyTreeCollection().getTechnology(technologyID).name;
			return technologyName;
			}			
			break;
		case ContinuousActionReference:
			return std::to_string(getContinuousActionID());
			break;
		case TileTypeReference:
			return getTileType(state).getName();
			break;
		default:
			return "Not defined";
		}
	}

	ActionTarget::ActionTarget(const Type& type, const Data& newData) : targetType(type), data(newData) {}
}
