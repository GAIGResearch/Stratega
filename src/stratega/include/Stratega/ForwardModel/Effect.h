#pragma once
#include <vector>
#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/ForwardModel/ActionTarget.h>

namespace SGA
{
	class ForwardModel;
	
	class Effect
	{
	public:
		virtual ~Effect() = default;

		virtual void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
		
		const std::string expr() const { return expression; };

	protected:
		Effect(const std::string exp) { expression = exp; }

	private:
		std::string expression;


	};

	class ModifyResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;
	public:
		ModifyResource(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class ApplyBuff: public Effect
	{
		FunctionParameter buffReference;
		FunctionParameter buffTicks;
        FunctionParameter entityParam;
	public:
		ApplyBuff(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class RemoveBuff: public Effect
	{
		FunctionParameter buffReference;
        FunctionParameter entityParam;
	public:
		RemoveBuff(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class RemoveAllBuffs : public Effect
	{
        FunctionParameter entityParam;
	public:
		RemoveAllBuffs(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class ChangeResource : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;
	public:
		ChangeResource(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Attack : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;
	public:
		Attack(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Empty :public Effect
	{
		FunctionParameter entityParam;
		//FunctionParameter targetPositionParam;
	public:
		Empty(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Push : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		Push(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

    class PushUp : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		PushUp(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

    class PushDown : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		PushDown(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

    class PushLeft : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		PushLeft(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

    class PushRight : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		PushRight(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class AttackProbability : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;
		FunctionParameter probabilityParameter;
	public:
		AttackProbability(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	
	class Move : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetPositionParam;
	public:
		Move(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	// ToDo This effect makes a lot of assumptions, for example what a valid position is or how large the spawn-area is. Additionally it doesn't work for RTS
	class SpawnEntityRandom : public Effect
	{
		FunctionParameter sourceEntityParam;
		FunctionParameter targetEntityTypeParam;

	public:
		SpawnEntityRandom(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	// ToDo This effect makes a lot of assumptions, for example what a valid position is or how large the spawn-area is. Additionally it doesn't work for RTS
	class SpawnEntityRandomLocation : public Effect
	{
		FunctionParameter targetEntityTypeParam;

	public:
		SpawnEntityRandomLocation(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	// ToDo This effect makes a lot of assumptions, for example what a valid position is or how large the spawn-area is. Additionally it doesn't work for RTS
	class SpawnEntity : public Effect
	{
		FunctionParameter spawnSource;
		FunctionParameter entityTypeParam;
		FunctionParameter targetPositionParam;

	public:
		SpawnEntity(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	// ToDo This effect makes a lot of assumptions, for example what a valid position is or how large the spawn-area is. Additionally it doesn't work for RTS
	class SpawnEntityGrid : public Effect
	{
		FunctionParameter spawnSource;
		FunctionParameter entityTypeParam;
		FunctionParameter targetPositionParam;

	public:
		SpawnEntityGrid(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SetToMaximum : public Effect
	{
		FunctionParameter targetResource;
		
	public:
		SetToMaximum(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class TransferEffect : public Effect
	{
		FunctionParameter sourceParam;
		FunctionParameter targetParam;
		FunctionParameter amountParam;

	public:
		TransferEffect(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class ChangeOwnerEffect : public Effect
	{
		FunctionParameter targetEntityParam;
		FunctionParameter playerParam;

	public:
		ChangeOwnerEffect(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class RemoveEntityEffect : public Effect
	{
		FunctionParameter targetEntityParam;

	public:
		RemoveEntityEffect(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	
	class ResearchTechnology : public Effect
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;

	public:
		ResearchTechnology(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class PayCostEffect : public Effect
	{
		FunctionParameter sourceParam;
		FunctionParameter costParam;

	public:
		PayCostEffect(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}