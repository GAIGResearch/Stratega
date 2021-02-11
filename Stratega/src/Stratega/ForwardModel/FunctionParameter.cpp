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

	FunctionParameter FunctionParameter::createEntityPlayerParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::EntityPlayerParameterReference, { .parameterData = ref });
	}
	
	FunctionParameter FunctionParameter::createEntityTypeReference(int entityTypeID)
	{
		return FunctionParameter(Type::EntityTypeReference, { .entityTypeID = entityTypeID});
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
			default:
				throw std::runtime_error("Type not recognised");
		}
	}

	const Parameter& FunctionParameter::getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if(actionTargets[data.parameterData.argumentIndex].getType()==ActionTarget::EntityReference)
			{
				auto& entity = getEntity(state, actionTargets);

				const auto& entityType = state.getEntityType(entity.typeID);
				const auto& param = entityType.getParameter(data.parameterData.parameterID);
				return param;
			}
			else if(actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& entity = getPlayer(state, actionTargets);
			
				const auto& param = state.getPlayerParameter(data.parameterData.parameterID);
				return param;
			}
			
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = state.playerParameterTypes->at(data.parameterData.parameterID);
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
				return entity.parameters[param.index];
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
			auto playerID = actionTargets[data.parameterData.argumentIndex].getPlayerID(state);
			return *state.getPlayer(playerID);
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

	const Player& FunctionParameter::getPlayer(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getPlayer(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	const EntityType& FunctionParameter::getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::EntityTypeReference)
		{
			return state.getEntityType(data.entityTypeID);
		}
		if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}
		
		throw std::runtime_error("Type not recognised");
	}

	const TechnologyTreeNode& FunctionParameter::getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		
		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return state.technologyTreeCollection->getTechnology(actionTarget.getTechnologyID());
		}
		else if (parameterType == Type::TechnologyTypeReference)
		{	
			return state.technologyTreeCollection->getTechnology(data.technologyTypeID);
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
				auto& sourceEntity = target.getEntity(const_cast<GameState&>(state));
				return sourceEntity.parameters;
			}
		}
		else
		{
			auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.parameters;
		}
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
				auto& sourceEntity = target.getEntity(const_cast<GameState&>(state));
				return sourceEntity.parameters;
			}
		}
		else
		{
			auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.parameters;
		}
	}

	const std::unordered_map<ParameterID, Parameter>& FunctionParameter::getParameterLookUp(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			return *state.playerParameterTypes;
			
		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				return *state.playerParameterTypes;
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = target.getEntity(const_cast<GameState&>(state));
				const auto& sourceEntityType = state.getEntityType(sourceEntity.typeID);
				return sourceEntityType.parameters;
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			const auto& sourceEntityType = state.getEntityType(sourceEntity.typeID);
			return sourceEntityType.parameters;
		}
	}

}