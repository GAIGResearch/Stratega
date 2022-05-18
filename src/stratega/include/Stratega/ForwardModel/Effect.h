#pragma once
#include <vector>
#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/ForwardModel/TargetType.h>
#include <Stratega/ForwardModel/SamplingMethod.h>
#include <boost/variant.hpp>
namespace SGA
{
	class ForwardModel;
	class Effect;
	class Condition;
	
	class EffectPack
	{
	public:
		enum EffectPackType
		{
			Sample,
			Conditional,
			Random
		};

		EffectPackType effectPackType;

		EffectPack(EffectPackType type):
			effectPackType(type)
		{
			
		}/*
		EffectPack()
		{
			
		}*/

		EffectPack() = delete;

		/// <summary>
		/// List of target types of this action with their conditions. Each target is a pair <target, list<conditions>>.
		/// These are tier 2 conditions (i.e. situational), to be checked during action generation.
		/// </summary>
		std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>> actionTargets;


		//Regular effect packs
		std::vector<boost::variant<std::shared_ptr<Effect>, std::shared_ptr<EffectPack>>> effects;

		//Condition effectPacks
		std::pair<std::vector<std::shared_ptr<Condition>>, std::vector<boost::variant<std::shared_ptr<Effect>, std::shared_ptr<EffectPack>>>> conditionEffects;

		//Random effectPacks
		std::vector<std::pair<float, std::vector<boost::variant<std::shared_ptr<Effect>, std::shared_ptr<EffectPack>>>>> randomEffects;

		//Execute method
		void execute(SGA::GameState& state, const SGA::ForwardModel& fm, const std::vector<SGA::ActionTarget>& targets);
	};
	
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

	class EnqueueAction : public Effect
	{
		FunctionParameter source;
		FunctionParameter actionType;
	public:
		EnqueueAction(const std::string exp, const std::vector<FunctionParameter>& parameters);
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

	class AttackWithArmorUnderCover : public Effect
	{
		FunctionParameter sourceReference;
		FunctionParameter armorReference;
		FunctionParameter healthReference;
		FunctionParameter amountParameter;
		FunctionParameter penaltyParameter;
	public:
		AttackWithArmorUnderCover(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class AttackAroundWithArmor : public Effect
	{
		FunctionParameter sourceReference;
		FunctionParameter targetPositionReference;
		FunctionParameter armorReference;
		FunctionParameter healthReference;
		FunctionParameter amountParameter;

		std::unique_ptr<Neighbours> samplingMethod;
	public:
		AttackAroundWithArmor(const std::string exp, const std::vector<FunctionParameter>& parameters);
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

	class PushAndHit : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;

		//Hit
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;
	public:
		PushAndHit(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class PushAroundPositionAndHit : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;

		//Hit
		FunctionParameter resourceReference;
		FunctionParameter amountParameter;

		std::unique_ptr<Neighbours> samplingMethod;
	public:
		PushAroundPositionAndHit(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class PickUp : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		PickUp(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class DropObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter objectParam;
		FunctionParameter positionParam;
	public:
		DropObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	
	class RemoveObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter objectParam;
	public:
		RemoveObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
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

	class EquipObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		EquipObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class UnEquipObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		UnEquipObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class UseObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		UseObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class UseSlotObject : public Effect
	{
		FunctionParameter entityParam;
		FunctionParameter targetParam;
	public:
		UseSlotObject(const std::string exp, const std::vector<FunctionParameter>& parameters);
		void execute(GameState& state, const ForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}