#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	FunctionParameter::FunctionParameter(double constValue):
		data{.constValue= constValue },
		parameterType(Type::Constant)
	{
	}
	
	FunctionParameter::FunctionParameter(int argumentIndex) :
		data{.argumentIndex = argumentIndex },
		parameterType(Type::ArgumentReference)
	{
	}
	
	FunctionParameter::FunctionParameter(ParameterID parameterID, int argumentIndex) :
		data{ .resourceData = ParameterReference(parameterID, argumentIndex) },
		parameterType(Type::ParameterReference)
	{
	}

	double FunctionParameter::getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			case Type::Constant: return data.constValue;
			case Type::ParameterReference: return getParameter(state, actionTargets);
			default:
				throw std::runtime_error("Type not recognised");
		}
	}
		
	double FunctionParameter::getParameter(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getParameter(const_cast<GameState&>(state), actionTargets);
	}
	
	double& FunctionParameter::getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if(parameterType == Type::ParameterReference)
		{
			auto entityID = std::get<int>(actionTargets[data.resourceData.argumentIndex]);
			auto& entity = state.entities[entityID];
			const auto& entityType = state.getEntityType(entity.typeID);
			auto iterator = state.entityTypes->find(entity.typeID);

			const auto& param = entityType.getParameter(data.resourceData.parameterID);
			return entity.parameters[param.index];
		}
		else
		{
			throw std::runtime_error("Type not recognized");
		}
	}
	
	Entity& FunctionParameter::getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
			case Type::ArgumentReference:
			{
				auto entityID = std::get<int>(actionTargets[data.resourceData.argumentIndex]);
				return state.getEntity(entityID);
			}
			default:
				throw std::runtime_error("Type not recognised");
		}
	}
	
	const Entity& FunctionParameter::getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getEntity(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}
}