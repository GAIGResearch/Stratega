#pragma once
#include <Stratega/ForwardModel/Action.h>
#include <Stratega/Representation/GameState.h> //TODO remove this

namespace SGA
{
	enum class ShapeType;

	class EntityActionSpace
	{
	public:
		virtual ~EntityActionSpace() = default;
		
		std::vector<Action> generateActions(const GameState& gameState, int player) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Entity& entity, const ActionType& action) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Player& entity, const ActionType& action) const;
		virtual std::vector<ActionTarget> generateEntityTypeTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs) const;
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, const Vector2f& position, ShapeType shape, int shapeSize) const;
     	virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState) const;
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs) const;
		virtual std::vector<ActionTarget> generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& technologyTypeIDs) const;
		virtual std::vector<ActionTarget> generateContinuousActionTargets(const GameState& gameState, const Entity& sourceEntity) const;
		virtual void generateActions(const GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;
		virtual void generateActions(const GameState& state, const Player& sourcePlayer, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;

		virtual Action generateSelfAction(const Entity& sourceEntity, const ActionType& actionType) const;
		virtual Action generateSelfAction(const Player& sourceEntity, const ActionType& actionType) const;		
	};
}