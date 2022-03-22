#pragma once
#include <Stratega/ForwardModel/Action.h>
#include <Stratega/Representation/GameState.h> //TODO remove this

namespace SGA
{
	enum class ShapeType;

	/// <summary>
	/// Test
	/// </summary>
	class ActionSpace
	{
	public:
		virtual ~ActionSpace() = default;
		
		std::vector<Action> generateActions(const GameState& gameState, int player) const;
		std::vector<Action> generateUnitActions(const GameState& gameState, Entity& e, int playerID, bool generateEnd = true) const;
		std::vector<Action> generatePlayerActions(const GameState& gameState, int playerID, bool generateEnd = true) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Entity& entity, const ActionType& action) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Player& entity, const ActionType& action) const;
		virtual std::vector<ActionTarget> generateEntityTypeTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs) const;

		//Entity
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, const Vector2f& position, std::shared_ptr<SamplingMethod> samplingMethod) const;
		//Player
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, std::shared_ptr<SamplingMethod> samplingMethod) const;
		
		//Entity
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;
		//Player
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;

		virtual std::vector<ActionTarget> generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& technologyTypeIDs) const;
		virtual std::vector<ActionTarget> generateContinuousActionTargets(const GameState& gameState, const Entity& sourceEntity) const;
		virtual void generateActions(const GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;
		virtual void generateActions(const GameState& state, const Player& sourcePlayer, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;

		virtual Action generateSelfAction(const Entity& sourceEntity, const ActionType& actionType) const;
		virtual Action generateSelfAction(const Player& sourceEntity, const ActionType& actionType) const;		
	};
}