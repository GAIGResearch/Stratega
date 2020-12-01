#pragma once
#include <vector>
#include <ForwardModel/AbstractFM/FunctionParameter.h>
#include <ForwardModel/AbstractFM/ActionTarget.h>

namespace SGA
{
	class Effect
	{
	public:
		Effect() = default;
		virtual ~Effect() = default;
		Effect(const Effect& other) = default;
		Effect(Effect&& other) noexcept = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(Effect&& other) noexcept = delete;
		
		virtual void execute(GameState& state, const std::vector<ActionTarget>& targets) const = 0;
	};

	class AddToResource: public Effect
	{
		FunctionParameter resourceReference;
		FunctionParameter amount;
	public:
		AddToResource(FunctionParameter resourceReference, FunctionParameter amount);
		void execute(GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class EndTurn : public Effect
	{
	public:
		void execute(GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}