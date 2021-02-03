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

	class ModifyResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		ModifyResource(const std::vector<FunctionParameter>& parameters);
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

	class Move : public Effect
	{
	public:
		Move(const std::vector<FunctionParameter>& parameters) {};
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SpawnUnit : public Effect
	{
		FunctionParameter entityTypeParam;
		FunctionParameter targetPositionParam;
		
	public:
		SpawnUnit(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	// ToDo This effect makes a lot of assumptions, for example what a valid position is or how large the spawn-area is. Additionally it doesn't work for RTS
	class SpawnEntityRandom : public Effect
	{
		FunctionParameter sourceEntityParam;
		FunctionParameter targetEntityTypeParam;

	public:
		SpawnEntityRandom(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SetToMaximum : public Effect
	{
		FunctionParameter targetResource;
		
	public:
		SetToMaximum(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class TransferEffect : public Effect
	{
		FunctionParameter sourceParam;
		FunctionParameter targetParam;
		FunctionParameter amountParam;

	public:
		TransferEffect(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class ChangeOwnerEffect : public Effect
	{
		FunctionParameter targetEntityParam;
		FunctionParameter playerParam;

	public:
		ChangeOwnerEffect(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class RemoveEntityEffect : public Effect
	{
		FunctionParameter targetEntityParam;

	public:
		RemoveEntityEffect(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	
	class ResearchTechnology : public Effect
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;

	public:
		ResearchTechnology(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class PayCostEffect : public Effect
	{
		FunctionParameter sourceParam;
		FunctionParameter costParam;

	public:
		PayCostEffect(const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const EntityForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}