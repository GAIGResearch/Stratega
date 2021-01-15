#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/FunctionParameter.h>

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
		HasResource(const std::vector<FunctionParameter>& parameters);

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class SamePlayer : public Condition
	{
	public:
		SamePlayer(const std::vector<FunctionParameter>& parameters);

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class InRange : public Condition
	{
		FunctionParameter sourceEntity;
		FunctionParameter targetEntity;
		FunctionParameter distance;

	public:
		InRange(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsWalkable : public Condition
	{
		FunctionParameter targetPosition;

	public:
		IsWalkable(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsPlayerEntity : public Condition
	{
		FunctionParameter targetParam;

	public:
		IsPlayerEntity(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class IsResearched : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;
		
	public:
		IsResearched(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class CanResearch : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;
		
	public:
		CanResearch(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}