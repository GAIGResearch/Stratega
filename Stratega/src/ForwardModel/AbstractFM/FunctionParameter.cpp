#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	double FunctionParameter::toValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const 
	{
		switch (parameterType)
		{
		case Type::Constant: return data.constValue; break;
		case Type::ResourceReference:
		{
			auto entityID = std::get<int>(actionTargets[data.resourceData.targetIndex]);
			auto& entity = state.entities[entityID];

			auto iterator= state.entityTypes->find(entity.typeID);
				
			if(iterator!=nullptr)
			{
				EntityType& entityType = iterator->second;

				auto parameterIterator=entityType.parameters.find(data.resourceData.resourceName);

				if(parameterIterator!=nullptr)
				{
					return entity.parameters[parameterIterator->second.index];
				}
				else
				{
					throw std::runtime_error("Parameter not found");
				}
			}
			else
			{
				throw std::runtime_error("EntityType not found");
			}
				
			
			
			break;
		}
		default:
			throw std::runtime_error("Type not recognised");
		}
	}

	double& FunctionParameter::getResourceReference(GameState& state, std::vector<ActionTarget>& actionTargets)
	{
		switch (parameterType)
		{
		case Type::Constant: return data.constValue; break;
		case Type::ResourceReference:
		{
			auto entityID = std::get<int>(actionTargets[data.resourceData.targetIndex]);
			auto& entity = state.getEntity(entityID);

			int index = state.findParameterIndex(data.resourceData.resourceName);
			if (index != -1)
				return entity.parameters[index];
			else
				throw std::runtime_error("Parameter index not found");
			break;
		}
		default:
			throw std::runtime_error("Type not recognised");
		}
	}
		
	Entity& FunctionParameter::getEntity(GameState& state, std::vector<ActionTarget>& actionTargets)
	{
		switch (parameterType)
		{
		case Type::TargetReference:
		{
			auto entityID = std::get<int>(actionTargets[data.resourceData.targetIndex]);
			return state.getEntity(entityID);
		}
		break;
		default:
			throw std::runtime_error("Type not recognised");
		}
	}

	const Entity& FunctionParameter::getEntity(GameState& state, std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::TargetReference:
		{
			auto entityID = std::get<int>(actionTargets[data.resourceData.targetIndex]);
			return state.getEntityConst(entityID);
		}
		break;
		default:
			throw std::runtime_error("Type not recognised");
		}
	}
}