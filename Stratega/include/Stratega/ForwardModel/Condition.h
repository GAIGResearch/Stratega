#pragma once
#include <Stratega/ForwardModel/Action.h>
#include <Stratega/ForwardModel/FunctionParameter.h>

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
	
	class ResourceLower : public Condition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBound;
	public:
		ResourceLower(const std::vector<FunctionParameter>& parameters);

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class ResourceGreater : public Condition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBound;
	public:
		ResourceGreater(const std::vector<FunctionParameter>& parameters);

		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class HasElapsedTime : public Condition
	{
		FunctionParameter lowerBound;
	public:
		HasElapsedTime(const std::vector<FunctionParameter>& parameters);

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

	class IsTile : public Condition
	{
		FunctionParameter targetPosition;
		FunctionParameter targetTile;

	public:
		IsTile(const std::vector<FunctionParameter>& parameters);
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

	class HasEntity : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter entityTypeParam;

	public:
		HasEntity(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class NoHasEntity : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter entityTypeParam;

	public:
		NoHasEntity(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	// ToDo This condition makes a lot of assumptions, mainly we had to add additional data to EntityType like RequiredTechnology and spawnableTypes
	class CanSpawnCondition : public Condition
	{
		FunctionParameter sourceEntityParam;
		FunctionParameter targetEntityTypeParam;

	public:
		CanSpawnCondition(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class CanAfford : public Condition
	{
		FunctionParameter sourceParam;
		FunctionParameter costParam;

	public:
		CanAfford(const std::vector<FunctionParameter>& parameters);
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}