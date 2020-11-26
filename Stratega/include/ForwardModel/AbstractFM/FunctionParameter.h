#pragma once
#include <string>
#include <vector>
#include <ForwardModel/AbstractFM/ActionTarget.h>
#include <Representation/AbstractGS/GameState.h>

namespace SGA
{
	class FunctionParameter
	{
		enum class Type
		{
			Constant,
			ResourceReference,
			TargetReference
		};

		struct ResourceReferenceData
		{
			std::string resourceName;
			int targetIndex;
		};

		union
		{
			double constValue;
			ResourceReferenceData resourceData;
			int targetIndex;
		} data;
		
		Type parameterType;

	public:
		double toValue(GameState& state, std::vector<ActionTarget>& actionTargets)
		{
			switch(parameterType)
			{
				case Type::Constant: return data.constValue; break;
				case Type::ResourceReference:
					auto entityID = std::get<int>(actionTargets[data.resourceData.targetIndex]);
					auto& entity = state.entities[entityID];
					return entity.parameters.find(data.resourceData.resourceName); break;
				default:
					throw std::runtime_error();
			}
		}
	};
}
