#pragma once
#include <ForwardModel/IActionSpace.h>
#include <ForwardModel/Action.h>
#include <Representation/GameState.h> // ToDo remove this shiat

namespace SGA
{
	class EntityActionSpace : public IActionSpace<GameState, Action>
	{
	public:
		std::vector<Action> generateActions(GameState& gameState) override { return {}; }
		std::vector<Action> generateActions(GameState& gameState, int player);

		std::vector<ActionTarget> generateTargets(const GameState& state, const Entity& entity, const ActionType& action);
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, const Vector2f& position, ShapeType shape, int shapeSize);
		virtual std::vector<ActionTarget> generateEntityTypeTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs);
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs);
		virtual std::vector<ActionTarget> generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& technologyTypeIDs);
		virtual void generateActions(const GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<ActionTarget>& targets, std::vector<Action>& actionBucket);
		virtual Action generateSelfAction(const Entity& sourceEntity, const ActionType& actionType);
	};
}