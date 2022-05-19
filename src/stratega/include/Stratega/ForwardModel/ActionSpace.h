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
		std::vector<Action> generateQueueActions(const GameState& gameState, int player) const;
		std::vector<Action> generateUnitActions(const GameState& gameState, Entity& e, int playerID, bool generateEnd = true) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Entity& entity, const ActionType& action) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Player& player, const ActionType& action) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Entity& entity, const std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>>& sourceTargets) const;
		std::vector<std::vector<ActionTarget>> generateTargets(const GameState& state, const Player& player, const std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>>& sourceTargets) const;
		virtual std::vector<ActionTarget> generateEntityTypeTargets(const GameState& gameState, const std::unordered_set<EntityTypeID>& entityTypeIDs) const;

		//Entity
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, const Vector2f& position, std::shared_ptr<SamplingMethod> samplingMethod) const;
		virtual std::vector<ActionTarget> generateTileTargets(const GameState& gameState, const Vector2f& position, const std::unordered_set<TileTypeID>& tileTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;
		
		//Player
		virtual std::vector<ActionTarget> generatePositionTargets(const GameState& gameState, std::shared_ptr<SamplingMethod> samplingMethod) const;
		virtual std::vector<ActionTarget> generateTileTargets(const GameState& gameState, const std::unordered_set<TileTypeID>& tileTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;
		
		//Entity
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;//Entity

		virtual std::vector<ActionTarget> generateObjectTargets(const GameState& gameState, const Entity& source, const std::unordered_set<EntityTypeID>& entityTypeIDs) const;
		virtual std::vector<ActionTarget> generateSlotObjectTargets(const GameState& gameState, const Entity& source, const std::unordered_set<EntityTypeID>& entityTypeIDs) const;

		//Player
		virtual std::vector<ActionTarget> generateGroupTargets(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs, std::shared_ptr<SamplingMethod> samplingMethod) const;

		virtual std::vector<ActionTarget> generateTechnologyTargets(const GameState& gameState, const std::unordered_set<int>& technologyTypeIDs) const;
		virtual std::vector<ActionTarget> generateContinuousActionTargets(const GameState& gameState, const Entity& sourceEntity) const;
		virtual void generateActions(const GameState& state, const Entity& sourceEntity, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;
		virtual void generateActions(const GameState& state, const Player& sourcePlayer, const ActionType& actionType, const std::vector<std::vector<ActionTarget>>& targets, std::vector<Action>& actionBucket) const;

		virtual Action generateSelfAction(const Entity& sourceEntity, const ActionType& actionType) const;
		virtual Action generateSelfAction(const Player& sourceEntity, const ActionType& actionType) const;	

		std::vector<std::vector<ActionTarget>> productActionTargets(const std::vector<std::vector<ActionTarget>>& lists) const;
	};
}