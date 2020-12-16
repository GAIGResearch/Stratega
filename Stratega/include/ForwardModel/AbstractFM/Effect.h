#pragma once
#include <vector>
#include <ForwardModel/AbstractFM/FunctionParameter.h>
#include <ForwardModel/AbstractFM/ActionTarget.h>
//#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>

namespace SGA
{
	class TBSAbstractForwardModel;
	class RTSAbstractForwardModel;
	
	class Effect
	{
	public:
		virtual ~Effect() = default;
		
		virtual void executeTBS(GameState& state,const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
		virtual void executeRTS(GameState& state,const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
	};

	class AddToResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		AddToResource(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Attack : public Effect
	{
		FunctionParameter entityTarget;
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		Attack(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	class RemoveFromResource : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		RemoveFromResource(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Move : public Effect
	{
	public:
		Move(const std::vector<FunctionParameter>& parameters) {};

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SpawnUnit : public Effect
	{
	public:

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}