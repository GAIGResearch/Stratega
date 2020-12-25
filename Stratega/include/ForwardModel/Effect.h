#pragma once
#include <vector>
#include <ForwardModel/FunctionParameter.h>
#include <ForwardModel/ActionTarget.h>
//#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{
	class TBSForwardModel;
	class RTSForwardModel;
	
	class Effect
	{
	public:
		virtual ~Effect() = default;
		
		virtual void executeTBS(GameState& state,const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
		virtual void executeRTS(GameState& state,const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const = 0;
	};

	class AddToResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		AddToResource(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Attack : public Effect
	{
		/*FunctionParameter entityTarget;*/
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		Attack(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
	class RemoveFromResource : public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		RemoveFromResource(const std::vector<FunctionParameter>& parameters);

		void executeTBS(GameState& state, const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Move : public Effect
	{
	public:
		Move(const std::vector<FunctionParameter>& parameters) {};

		void executeTBS(GameState& state, const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class SpawnUnit : public Effect
	{
	public:
		SpawnUnit(const std::vector<FunctionParameter>& parameters);
		void executeTBS(GameState& state, const TBSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}