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
		Effect() = default;
		virtual ~Effect() = default;
		Effect(const Effect& other) = default;
		Effect(Effect&& other) noexcept = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(Effect&& other) noexcept = delete;
		
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
}