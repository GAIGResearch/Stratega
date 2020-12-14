#pragma once
#include <ForwardModel/AbstractFM/Action.h>
#include <ForwardModel/AbstractFM/FunctionParameter.h>

namespace SGA
{
	class Condition
	{
	public:
		Condition() = default;
		virtual ~Condition() = default;
		Condition(const Condition& other) = default;
		Condition(Condition&& other) noexcept = delete;
		Condition& operator=(const Condition& other) = delete;
		Condition& operator=(Condition&& other) noexcept = delete;
	
		virtual bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const = 0;
	};

	class HasResource : public Condition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBound;
	public:
		HasResource(FunctionParameter resourceReference, FunctionParameter lowerBound);

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class SamePlayer : public Condition
	{
		
	public:
		SamePlayer();

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
}