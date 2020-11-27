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

			ResourceReferenceData(int targetIndex, std::string resourceName):
			resourceName(resourceName),
			targetIndex(targetIndex)
			{
				
			}
		};

		union
		{
			double constValue;
			ResourceReferenceData resourceData;
			int targetIndex;
		} data;
		
		Type parameterType;

		FunctionParameter(double constValue):
		parameterType(Type::Constant),
			data{.constValue= constValue }
		{
			
		}

		FunctionParameter(int targetIndex) :
			parameterType(Type::TargetReference),
			data{.targetIndex = targetIndex }
		{

		}

		FunctionParameter(int targetIndex,std::string resourceName) :
			parameterType(Type::TargetReference),
			data{ .resourceData = ResourceReferenceData(targetIndex,resourceName) }
		{
			data.targetIndex = targetIndex;
		}
		
	public:
		double toValue(const GameState& state, const std::vector<ActionTarget>& actionTargets)const;		

		double& getResourceReference(GameState& state, std::vector<ActionTarget>& actionTargets);
		
		Entity& getEntity(GameState& state, std::vector<ActionTarget>& actionTargets);
		const Entity& getEntity(GameState& state, std::vector<ActionTarget>& actionTargets) const;
		
	};
}
