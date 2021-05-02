#pragma once
#include <random>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/EntityActionSpace.h>
#include <Stratega/ForwardModel/ActionAssignment.h>

#include "Condition.h"
#include "Effect.h"

namespace SGA
{
	enum class WinConditionType
	{
		LastManStanding,
		UnitAlive
	};
	
	struct OnTickEffect
	{
		std::unordered_set<EntityTypeID> validTargets;
		std::vector<std::shared_ptr<Condition>> conditions;
		std::vector<std::shared_ptr<Effect>> effects;
	};

	struct OnEntitySpawnEffect
	{
		std::unordered_set<EntityTypeID> validTargets;
		std::vector<std::shared_ptr<Condition>> conditions;
		std::vector<std::shared_ptr<Effect>> effects;
	};
	
	class EntityForwardModel
	{
	public:
		std::shared_ptr<EntityActionSpace> actionSpace;
		std::vector<OnTickEffect> onTickEffects;
		std::vector<OnEntitySpawnEffect> onEntitySpawnEffects;

		//Conditions
		std::vector<std::vector<std::shared_ptr<Condition>>> winConditions;
		std::vector<std::vector<std::shared_ptr<Condition>>> loseConditions;
		
		std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>> actionTargets;
						
		virtual ~EntityForwardModel() = default;
		EntityForwardModel();
		virtual void advanceGameState(GameState& state, const Action& action) const = 0;
		virtual void advanceGameState(GameState& state, const ActionAssignment& action) const = 0;
		[[nodiscard]] std::vector<Action> generateActions(const GameState& state, int playerID) const;
		virtual void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const;
		virtual std::unique_ptr<EntityForwardModel> clone() const = 0;
		
		bool canPlayerPlay(const GameState& state, Player& player) const;
		bool checkPlayerWon(const GameState& state, Player& player) const;
		void executeAction(GameState& state, const Action& action) const;
		void endTick(GameState& state) const;
		void spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const;

		std::unique_ptr<EntityActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<EntityActionSpace>();
		}
		
		void setActionSpace(std::unique_ptr<EntityActionSpace> newActionSpace)
		{
			this->actionSpace = std::move(newActionSpace);
		}
	};

}
