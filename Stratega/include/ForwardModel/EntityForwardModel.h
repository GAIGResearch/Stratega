#pragma once
#include <Representation/GameState.h>
// ToDo Why is this in configuration?
#include <Configuration/WinConditionType.h>

#include <ForwardModel/EntityActionSpace.h>

namespace SGA
{
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

		WinConditionType winCondition;
		int targetUnitTypeID;

		virtual ~EntityForwardModel() = default;
		EntityForwardModel()
			: winCondition(WinConditionType::LastManStanding),
			  targetUnitTypeID(-1)
		{
		}
		
		bool canPlayerPlay(const GameState& state, Player& player) const;
		void executeAction(GameState& state, const Action& action) const;
		void endTick(GameState& state) const;
		void spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const;

		std::unique_ptr<EntityActionSpace> generateDefaultActionSpace()
		{
			return std::make_unique<EntityActionSpace>();
		}
		
		void setActionSpace(std::unique_ptr<EntityActionSpace> actionSpace)
		{
			this->actionSpace = std::move(actionSpace);
		}
	};

}
