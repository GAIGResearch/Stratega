#pragma once
#include <vector>
#include <ForwardModel/FunctionParameter.h>
#include <ForwardModel/ActionTarget.h>

namespace SGA
{
	class EntityForwardModel;
	
	class Effect
	{
	public:
		virtual ~Effect() = default;
		
		virtual void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
	};

	class AddToResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		AddToResource(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Attack : public Effect
	{
		/*FunctionParameter entityTarget;*/
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		Attack(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	class RemoveFromResource : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		RemoveFromResource(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Move : public Effect
	{
	public:
		Move(const std::vector<FunctionParameter>& parameters) {};
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SpawnUnit : public Effect
	{
	public:
		SpawnUnit(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}