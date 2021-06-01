#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	FunctionParameter FunctionParameter::createConstParameter(double constValue)
	{
		return FunctionParameter(Type::Constant, { .constValue = constValue });
	}
	
	FunctionParameter FunctionParameter::createArgumentReference(int argumentIndex)
	{
		return FunctionParameter(Type::ArgumentReference, {.argumentIndex = argumentIndex});
	}
	
	FunctionParameter FunctionParameter::createParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::ParameterReference, { .parameterData = ref});
	}

	FunctionParameter FunctionParameter::createEntityPlayerReference(int argumentIndex)
	{
		return FunctionParameter(Type::EntityPlayerReference, { .argumentIndex = argumentIndex });
	}

	FunctionParameter FunctionParameter::createTimeReference(int argumentIndex)
	{
		return FunctionParameter(Type::TimeReference, { .argumentIndex = argumentIndex });
	}

	FunctionParameter FunctionParameter::createEntityPlayerParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::EntityPlayerParameterReference, { .parameterData = ref });
	}
	
	FunctionParameter FunctionParameter::createEntityTypeReference(int entityTypeID)
	{
		return FunctionParameter(Type::EntityTypeReference, { .entityTypeID = entityTypeID});
	}

	FunctionParameter FunctionParameter::createTileTypeReference(int tileTypeID)
	{
		return FunctionParameter(Type::TileTypeReference, { .tileTypeID = tileTypeID });
	}

	FunctionParameter FunctionParameter::createTechnologyTypeReference(int technologyTypeID)
	{
		return FunctionParameter(Type::TechnologyTypeReference, { .technologyTypeID = technologyTypeID });
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

		throw std::runtime_error("Type not recognised");
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
				throw std::runtime_error("Type not recognised");
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
				return state.gameInfo->technologyTreeCollection->getTechnology(technologyID).continuousActionTime;
			}
			else if (target.getType() == ActionTarget::EntityReference
				|| target.getType() == ActionTarget::EntityTypeReference)
			{
				auto& entityType = target.getEntityType(state);
				return entityType.continuousActionTime;
			}

		}
		
		throw std::runtime_error("Type not recognized");
	}

	const Parameter& FunctionParameter::getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType()==ActionTarget::EntityReference)
			{
				auto& entity = getEntity(state, actionTargets);

				const auto& entityType = *entity.type;
				const auto& param = entityType.getParameter(data.parameterData.parameterID);
				return param;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				const auto& param = state.gameInfo->getPlayerParameter(data.parameterData.parameterID);

				return param;
			}
			
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = state.gameInfo->playerParameterTypes->at(data.parameterData.parameterID);
			return param;
		}

		
		throw std::runtime_error("Type not recognized");
	}
	
	double FunctionParameter::getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getParameterValue(const_cast<GameState&>(state), actionTargets);
	}

	double& FunctionParameter::getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				auto& entity = getEntity(state, actionTargets);
				return entity.getParameterAt(param.index);
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& player = getPlayer(state, actionTargets);
				return player.parameters[param.index];
			}
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			auto& entity = getEntity(state, actionTargets);
			auto* player = state.getPlayer(entity.ownerID);
			return player->parameters[param.index];
		}

		throw std::runtime_error("Type not recognized");
	}

	Vector2f FunctionParameter::getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ArgumentReference)
		{
			return actionTargets[data.argumentIndex].getPosition(state);
		}
		else
		{
			throw std::runtime_error("Type not recognised");
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
				throw std::runtime_error("Type not recognised");
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
			return *state.getPlayer(entity.ownerID);
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayer(state);
		}
		default:
			throw std::runtime_error("Type not recognised");
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
			return entity.ownerID;
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayerID(state);
		}
		default:
			throw std::runtime_error("Type not recognised");
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
			return state.gameInfo->getEntityType(data.entityTypeID);
		}
		if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}
		
		throw std::runtime_error("Type not recognised");
	}

	const std::unordered_set<EntityTypeID>& FunctionParameter::getSpawneableEntities(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::ParameterReference:
		{
			return actionTargets[data.parameterData.argumentIndex].getSpawneableEntities(state);
		}
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			const auto& entity = getEntityType(state, actionTargets);
			return entity.spawnableEntityTypes;
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getSpawneableEntities(state);
		}
		default:
			throw std::runtime_error("Type not recognised");
		}
	}

	const TechnologyTreeNode& FunctionParameter::getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		
		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return state.gameInfo->technologyTreeCollection->getTechnology(actionTarget.getTechnologyID());
		}
		else if (parameterType == Type::TechnologyTypeReference)
		{	
			return state.gameInfo->technologyTreeCollection->getTechnology(data.technologyTypeID);
		}
		else
		{
			throw std::runtime_error("Type not recognized");
		}
		
	}

	const std::unordered_map<ParameterID, double>& FunctionParameter::getCost(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			if(actionTarget.getType() == ActionTarget::EntityTypeReference)
			{
				return getEntityType(state, actionTargets).cost;
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
			return getEntityType(state, actionTargets).cost;
		}

		throw std::runtime_error("Type not recognized");
	}

	std::vector<double>& FunctionParameter::getParameterList(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.parameters;

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(const_cast<GameState&>(state));
				return player.parameters;
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				return sourceEntity.getParamValues();
			}
		}
		else
		{
			auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.getParamValues();
		}

		throw std::runtime_error("Type not recognized");
	}

	const std::vector<double>& FunctionParameter::getParameterList(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.parameters;

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(const_cast<GameState&>(state));
				return player.parameters;
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

		throw std::runtime_error("Tried accessing ParameterMap of invalid parameter");
	}

	const std::unordered_map<ParameterID, Parameter>& FunctionParameter::getParameterLookUp(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			return *state.gameInfo->playerParameterTypes;
			
		}
		if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				return *state.gameInfo->playerParameterTypes;
			}
			if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				const auto& sourceEntityType = *sourceEntity.type;
				return sourceEntityType.parameters;
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			const auto& sourceEntityType = *sourceEntity.type;
			return sourceEntityType.parameters;
		}

		throw std::runtime_error("Tried accessing ParameterMap of invalid parameter");
	}

	const TileType& FunctionParameter::getTileType(const GameState& state, const std::vector<ActionTarget>& /*actionTargets*/) const
	{
		
		if (parameterType == Type::TileTypeReference)
		{
			return state.gameInfo->getTileType(data.tileTypeID);
		}

		throw std::runtime_error("Type not recognised");
	}

}