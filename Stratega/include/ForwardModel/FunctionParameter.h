#pragma once
#include <string>
#include <vector>
#include <ForwardModel/ActionTarget.h>
#include <Representation/EntityType.h>

namespace SGA
{
	struct GameState;
	
	class FunctionParameter
	{
		enum class Type
		{
			Constant,
			ParameterReference,
			ArgumentReference, //Entity o position
			EntityTypeReference
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

		union Data
		{
			double constValue;
			ParameterReference parameterData;
			int argumentIndex;
			int entityTypeID;
		};
		
		Type parameterType;
		Data data;

		// Private since this class should only be constructed using the static methods
		FunctionParameter(const Type& type, const Data& data) : parameterType(type), data(data) {};

	public:
		static FunctionParameter createConstParameter(double constValue);
		static FunctionParameter createArgumentReference(int argumentIndex);
		static FunctionParameter createParameterReference(ParameterReference ref);
		static FunctionParameter createEntityTypeReference(int entityTypeID);

		double getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Parameter& getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double& getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Vector2f getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const EntityType& getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		
	};
}
