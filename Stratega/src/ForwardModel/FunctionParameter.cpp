#include <ForwardModel/FunctionParameter.h>

#include <Representation/GameState.h>
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
			auto& entity = getEntity(state, actionTargets);
			const auto& entityType = state.getEntityType(entity.typeID);
			const auto& param = entityType.getParameter(data.parameterData.parameterID);
			return param;
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
			auto& entity = getEntity(state, actionTargets);
			return entity.parameters[param.index];
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
			return actionTargets[data.argumentIndex].getPosition();
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

	Player& FunctionParameter::getPlayer(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			case Type::EntityPlayerParameterReference:
			case Type::EntityPlayerReference:
			{
				auto& entity = getEntity(state, actionTargets);
				return *state.getPlayer(entity.ownerID);
			}
			default:
				throw std::runtime_error("Type not recognised");
		}
	}
	
	const Entity& FunctionParameter::getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getEntity(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	const EntityType& FunctionParameter::getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::EntityTypeReference)
		{
			return state.getEntityType(data.entityTypeID);
		}
		else
		{
			throw std::runtime_error("Type not recognised");
		}
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
	
}