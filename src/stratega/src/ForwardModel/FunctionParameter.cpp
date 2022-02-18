#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	FunctionParameter FunctionParameter::createConstParameter(double constValue)
	{
		return FunctionParameter(Type::Constant, { constValue });
	}
	
	FunctionParameter FunctionParameter::createArgumentReference(int argumentIndex)
	{
		return FunctionParameter(Type::ArgumentReference, { Type::ArgumentReference, argumentIndex});
	}
	
	FunctionParameter FunctionParameter::createParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::ParameterReference, {ref});
	}

	FunctionParameter FunctionParameter::createEntityPlayerReference(int argumentIndex)
	{
		return FunctionParameter(Type::EntityPlayerReference, { Type::EntityPlayerReference, argumentIndex });
	}

	FunctionParameter FunctionParameter::createTimeReference(int argumentIndex)
	{
		return FunctionParameter(Type::TimeReference, { Type::TimeReference, argumentIndex });
	}

	FunctionParameter FunctionParameter::createEntityPlayerParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::EntityPlayerParameterReference, { ref });
	}

	FunctionParameter FunctionParameter::createGameStateParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::GameStateParameterReference, { ref });
	}
	
	FunctionParameter FunctionParameter::createEntityTypeReference(int entityTypeID)
	{
		return FunctionParameter(Type::EntityTypeReference, { Type::EntityTypeReference, entityTypeID});
	}

	FunctionParameter FunctionParameter::createTileTypeReference(int tileTypeID)
	{
		return FunctionParameter(Type::TileTypeReference, { Type::TileTypeReference, tileTypeID });
	}

	FunctionParameter FunctionParameter::createTechnologyTypeReference(int technologyTypeID)
	{
		return FunctionParameter(Type::TechnologyTypeReference, { Type::TechnologyTypeReference, technologyTypeID });
	}
	FunctionParameter FunctionParameter::createBuffTypeReference(int buffTypeID)
	{
		return FunctionParameter(Type::BuffTypeReference, { Type::BuffTypeReference, buffTypeID });
	}

	FunctionParameter::Type FunctionParameter::getType() const
	{
		return parameterType;
	}

	// ToDo Remove this
	const ActionTarget& FunctionParameter::getActionTarget(const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ArgumentReference)
		{
			return actionTargets[data.argumentIndex];
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	double FunctionParameter::getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			case Type::Constant: return data.constValue;
			case Type::ParameterReference:
			case Type::EntityPlayerParameterReference: return getParameterValue(state, actionTargets);
			case Type::TimeReference: return getTime(state, actionTargets);
			default:
				throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}
	
	double FunctionParameter::getTime(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::TimeReference)
		{
			auto& target = actionTargets[data.argumentIndex];
			if (target.getType() == ActionTarget::TechnologyReference)
			{
				auto technologyID = target.getTechnologyID();
				return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(technologyID).continuousActionTime;
			}
			else if (target.getType() == ActionTarget::EntityReference
				|| target.getType() == ActionTarget::EntityTypeReference)
			{
				auto& entityType = target.getEntityType(state);
				return entityType.getContinuousActionTime();
			}

		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const Parameter& FunctionParameter::getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType()==ActionTarget::EntityReference)
			{
				auto& entity = getEntity(state, actionTargets);

				const auto& entityType = entity.getEntityType();
				const auto& param = entityType.getParameter(data.parameterData.parameterID);
				return param;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				const auto& param = state.getGameInfo()->getPlayerParameter(data.parameterData.parameterID);

				return param;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				const auto& param = state.getGameInfo()->getStateParameter(data.parameterData.parameterID);

				return param;
			}
			
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = state.getGameInfo()->getPlayerParameterTypes().at(data.parameterData.parameterID);
			return param;
		}
		if(parameterType == Type::GameStateParameterReference)
		{
			const auto& param = state.getGameInfo()->getStateParameterTypes().at(data.parameterData.parameterID);
			return param;
		}


		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	
	bool FunctionParameter::isPlayerParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType()==ActionTarget::EntityReference)
			{
				return false;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				return true;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return false;
			}
			
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			return true;
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			return false;
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	
	bool FunctionParameter::isEntityParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType()==ActionTarget::EntityReference)
			{
				return true;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				return false;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return false;
			}
			
		}
		if(parameterType == Type::EntityPlayerParameterReference|| parameterType == Type::GameStateParameterReference)
		{
			return false;
		}


		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	
	bool FunctionParameter::isStateParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return true;
			}
			return false;
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			return true;
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	double FunctionParameter::getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
           return getRawParameterValue(const_cast<GameState& >(state), actionTargets);
		/*if(parameterType == Type::ParameterReference)
		{
			const auto& param = getParameter(const_cast<GameState&>(state), actionTargets);
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				const auto& entity = getEntity(state, actionTargets);
				return entity.getParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& player = getPlayer(state, actionTargets);
				return player.getParameter(param.getIndex());
			}
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = getParameter(const_cast<GameState&>(state), actionTargets);
			auto& entity = getEntity(state, actionTargets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return player->getParameter(param.getIndex());
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");*/
	}

	double FunctionParameter::getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getRawParameterValue(const_cast< GameState&>(state), actionTargets);
	}

	double& FunctionParameter::getRawParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				//std::cout <<"		" << param.getName()<<" "<<std::endl;
				auto& entity = getEntity(state, actionTargets);
				return entity.getRawParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& player = getPlayer(state, actionTargets);
				return player.getRawParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return state.getRawParameterAt(param.getIndex());
			}
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			auto& entity = getEntity(state, actionTargets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return player->getRawParameterAt(param.getIndex());
		}
		if(parameterType == Type::GameStateParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			return state.getRawParameterAt(param.getIndex());
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	Vector2f FunctionParameter::getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ArgumentReference)
		{
			return actionTargets[data.argumentIndex].getPosition(state);
		}
		else
		{
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}
	
	Entity& FunctionParameter::getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			case Type::EntityPlayerReference:
			case Type::ArgumentReference:
			{
				auto entityID = actionTargets[data.argumentIndex].getEntityID();
				return *state.getEntity(entityID);
			}
			case Type::ParameterReference:
			case Type::EntityPlayerParameterReference:
			{
				auto entityID = actionTargets[data.parameterData.argumentIndex].getEntityID();
				return *state.getEntity(entityID);
			}
			default:
				throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const Entity& FunctionParameter::getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getEntity(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	Player& FunctionParameter::getPlayer(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			
		case Type::ParameterReference:
		{
			return actionTargets[data.parameterData.argumentIndex].getPlayer(state);
		}
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			auto& entity = getEntity(state, actionTargets);
			return *state.getPlayer(entity.getOwnerID());
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayer(state);
		}
		case Type::Constant:
		{
			return *state.getPlayer(static_cast<int>(data.constValue));
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}


	int FunctionParameter::getPlayerID(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			const auto& entity = getEntity(state, actionTargets);
			return entity.getOwnerID();
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayerID(state);
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}
	
	const Player& FunctionParameter::getPlayer(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getPlayer(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	const EntityType& FunctionParameter::getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::EntityTypeReference)
		{
			return state.getGameInfo()->getEntityType(data.entityTypeID);
		}
		if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	const BuffType& FunctionParameter::getBuffType(const GameState& state, const std::vector<ActionTarget>& /*actionTargets*/) const
	{
		if(parameterType == Type::BuffTypeReference)
		{
			return state.getGameInfo()->getBuffType(data.buffTypeID);
		}
		/*if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}*/

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_set<EntityTypeID>& FunctionParameter::getSpawnableEntities(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::ParameterReference:
		{
			return actionTargets[data.parameterData.argumentIndex].getSpawnableEntities(state);
		}
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			const auto& entity = getEntityType(state, actionTargets);
			return entity.getSpawnableEntityTypes();
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getSpawnableEntities(state);
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const TechnologyTreeNode& FunctionParameter::getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		
		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(actionTarget.getTechnologyID());
		}
		else if (parameterType == Type::TechnologyTypeReference)
		{	
			return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(data.technologyTypeID);
		}
		else
		{
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
		
	}

	const std::unordered_map<ParameterID, double>& FunctionParameter::getCost(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			if(actionTarget.getType() == ActionTarget::EntityTypeReference)
			{
				return getEntityType(state, actionTargets).getCosts();
			}
			else if(actionTarget.getType() == ActionTarget::TechnologyReference)
			{
				return getTechnology(state, actionTargets).cost;
			}
		}
		else if(parameterType == Type::TechnologyTypeReference)
		{
			return getTechnology(state, actionTargets).cost;
		}
		else if(parameterType == Type::EntityTypeReference)
		{
			return getEntityType(state, actionTargets).getCosts();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	std::unordered_map<int, double>& FunctionParameter::getParameterList(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.getParameters();

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(state);
				return player.getParameters();
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(state);
				return sourceEntity.getParamValues();
			}
		}
		else
		{
			auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.getParamValues();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_map<int, double>& FunctionParameter::getParameterList(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.getParameters();

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(const_cast<GameState&>(state));
				return player.getParameters();
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				return sourceEntity.getParamValues();
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.getParamValues();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_map<ParameterID, Parameter>& FunctionParameter::getParameterLookUp(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			return state.getGameInfo()->getPlayerParameterTypes();
			
		}
		if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				return state.getGameInfo()->getPlayerParameterTypes();
			}
			if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				const auto& sourceEntityType = sourceEntity.getEntityType();
				return sourceEntityType.getParameters();
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			const auto& sourceEntityType = sourceEntity.getEntityType();
			return sourceEntityType.getParameters();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const TileType& FunctionParameter::getTileType(const GameState& state, const std::vector<ActionTarget>& /*actionTargets*/) const
	{
		if (parameterType == Type::TileTypeReference)
		{
			return state.getGameInfo()->getTileType(data.tileTypeID);
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

}