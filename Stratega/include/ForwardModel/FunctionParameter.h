#pragma once
#include <vector>
#include <ForwardModel/ActionTarget.h>
#include <Representation/EntityType.h>
#include <Representation/Player.h>
#include <Representation/TechnologyTree.h>

namespace SGA
{
	struct GameState;
	
	class FunctionParameter
	{
		enum class Type
		{
			Constant, // Values like 1, 1.5, etc
			ParameterReference, // References the parameter of an action-target. Source.Gold references to the gold of the source of an action
			EntityPlayerReference, // References the player indirectly. Like Source.Player, only works if Source is a entity
			EntityPlayerParameterReference, // References the parameter of an player indirectly. Like Source.Player.Gold, only works if Source is a entity
			ArgumentReference, // References an action-argument, like Source or Target. Can be used for example if you want to pass a position-target
			EntityTypeReference, // References EntityTypes defined in the Game. Like Warrior, GoldMine, etc.
			TechnologyTypeReference, // References TechnologyTypes defined in the Game 
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
			int technologyTypeID;
		};
		
		Type parameterType;
		Data data;

		// Private since this class should only be constructed using the static methods
		FunctionParameter(const Type& type, const Data& data) : parameterType(type), data(data) {};

	public:
		static FunctionParameter createConstParameter(double constValue);
		static FunctionParameter createArgumentReference(int argumentIndex);
		static FunctionParameter createParameterReference(ParameterReference ref);
		static FunctionParameter createEntityPlayerReference(int argumentIndex);
		static FunctionParameter createEntityPlayerParameterReference(ParameterReference ref);
		static FunctionParameter createEntityTypeReference(int entityTypeID);
		static FunctionParameter createTechnologyTypeReference(int technologyTypeID);

		double getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Parameter& getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double& getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Vector2f getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Player& getPlayer(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const EntityType& getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const TechnologyTreeNode& getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		
	};
}
