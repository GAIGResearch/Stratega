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
	
		virtual bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const = 0;
		const std::string expr() const { return expression; };

	protected:
		Condition(const std::string exp) { expression = exp; }

	private:
		std::string expression;
	};
	
	class ResourceLowerEqual : public Condition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBoundParameter;
	public:
		ResourceLowerEqual(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class ResourceGreaterEqual : public Condition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBoundParameter;
	public:
		ResourceGreaterEqual(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class HasElapsedTime : public Condition
	{
		FunctionParameter lowerBoundParameter;
	public:
		HasElapsedTime(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class IsNeutral : public Condition
	{
	public:
		IsNeutral(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsNotNeutral : public Condition
	{
	public:
		IsNotNeutral(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class SamePlayer : public Condition
	{
	public:
		SamePlayer(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class DifferentPlayer : public Condition
	{
	public:
		DifferentPlayer(const std::string exp, const std::vector<FunctionParameter>& parameters);

		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class InRange : public Condition
	{
		FunctionParameter sourceEntity;
		FunctionParameter targetEntity;
		FunctionParameter distance;

	public:
		InRange(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsWalkable : public Condition
	{
		FunctionParameter targetPosition;

	public:
		IsWalkable(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsTile : public Condition
	{
		FunctionParameter targetPosition;
		FunctionParameter targetTile;

	public:
		IsTile(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class IsPlayerEntity : public Condition
	{
		FunctionParameter targetParam;

	public:
		IsPlayerEntity(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class IsTickMultipleOf : public Condition
	{
		FunctionParameter multipleParam;

	public:
		IsTickMultipleOf(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class IsResearched : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;
		
	public:
		IsResearched(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class CanResearch : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter technologyTypeParam;
		
	public:
		CanResearch(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class HasEntity : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter entityTypeParam;

	public:
		HasEntity(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class HasNoEntity : public Condition
	{
		FunctionParameter playerParam;
		FunctionParameter entityTypeParam;

	public:
		HasNoEntity(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;

	};

	class HasNoBuff : public Condition
	{
		FunctionParameter entityParam;
		FunctionParameter buffTypeParam;

	public:
		HasNoBuff(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	class HasBuff : public Condition
	{
		FunctionParameter entityParam;
		FunctionParameter buffTypeParam;

	public:
		HasBuff(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
	
	// ToDo This condition makes a lot of assumptions, mainly we had to add additional data to EntityType like RequiredTechnology and spawnableTypes
	class CanSpawnCondition : public Condition
	{
		FunctionParameter sourceEntityParam;
		FunctionParameter targetEntityTypeParam;

	public:
		CanSpawnCondition(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};

	class CanAfford : public Condition
	{
		FunctionParameter sourceParam;
		FunctionParameter costParam;

	public:
		CanAfford(const std::string exp, const std::vector<FunctionParameter>& parameters);
		bool isFullfiled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}