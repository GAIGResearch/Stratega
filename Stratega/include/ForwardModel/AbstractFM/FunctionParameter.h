#pragma once
#include <string>
#include <vector>
#include <ForwardModel/AbstractFM/ActionTarget.h>
#include <Representation/AbstractGS/EntityType.h>

namespace SGA
{
	class GameState;
	
	class FunctionParameter
	{
		enum class Type
		{
			Constant,
			ParameterReference,
			ArgumentReference
		};

		struct ParameterReference
		{
			ParameterID parameterID;
			int argumentIndex;

			ParameterReference(ParameterID parameterID, int argumentIndex):
				parameterID(parameterID),
				argumentIndex(argumentIndex)
			{
			}
		};

		union
		{
			double constValue;
			ParameterReference resourceData;
			int argumentIndex;
		} data;
		
		Type parameterType;

	public:
		FunctionParameter(double constValue);
		FunctionParameter(int argumentIndex);
		FunctionParameter(ParameterID parameterID, int argumentIndex);

		double getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getParameter(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double& getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		
	};
}
