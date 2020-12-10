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
		AddToResource(FunctionParameter resourceReference, FunctionParameter amount);

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class EndTurn : public Effect
	{
	public:

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};

	class Move : public Effect
	{
	public:

		void executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
		void executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const override;
	};
}