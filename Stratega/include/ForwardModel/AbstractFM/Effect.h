#pragma once
#include <vector>
#include <ForwardModel/AbstractFM/FunctionParameter.h>
#include <ForwardModel/AbstractFM/ActionTarget.h>

namespace SGA
{
	class Effect
	{
	public:
		virtual ~Effect() = default;
		
		virtual void execute(GameState& state, const std::vector<ActionTarget>& targets) const = 0;
	};

	class AddToResource: Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		AddToResource(FunctionParameter resourceReference, FunctionParameter amount);
		void execute(GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}