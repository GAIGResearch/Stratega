#pragma once
#include <random>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/EntityActionSpace.h>

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

	/// <summary>
	/// Is the driving component of Stratega, with a provided gamestate it can generate a new set of available
	/// actions thanks to the <see cref="SGA::EntityActionSpace"/>.
	/// The returned actions store the action type and a list of sources and targets the action will be applied to.
	/// 
	/// Each <see cref="SGA::RTSForwardModel"/> and <see cref="SGA::TBSForwardModel"/> by default contains the definition of how to advance the game
	/// and other utilities methods used in each specific game type.
	/// 
	/// When a player provides the forward model an action and a gamestate, it first checks the type of action
	/// and after it updates the action info before executing the action.
	/// 
	/// The forward models can be extended by implementing forward model abstractions, can be useful for reducing the game’s
	/// complexity and ease the training and decision-making process of AI agents. 
	/// </summary>
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
		virtual void advanceGameState(GameState& state, const std::vector<Action>& action) const = 0;

		/// <summary>
		/// Returns a list of available actions in the given gamestate by the received player.
		/// </summary>
		[[nodiscard]] std::vector<Action> generateActions(const GameState& state, int playerID) const;
		/// <summary>
		/// Updated a list of available actions in the given gamestate by the received player.
		/// </summary>
		virtual void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const;

		/// <summary>
		/// Returns if a player can continue play in the current game.
		/// Each game configuration can have lose conditions that define the rules to lose a game.
		/// </summary>
		bool canPlayerPlay(const GameState& state, Player& player) const;
		/// <summary>
		/// Returns if a player won the game
		/// Each game configuration can have win conditions that define the rules to win a game.
		/// </summary>
		bool checkPlayerWon(const GameState& state, Player& player) const;

		/// <summary>
		/// Executes the action in the given gamestate
		/// </summary>
		void executeAction(GameState& state, const Action& action) const;
		/// <summary>
		/// After each turn/ tick the triggers from the configuration are executed and
		/// the continuous action are executed after checking if they still are feasible.
		/// In terms of real-time strategy games, we consider a turn/tick to be the
		/// time-span until an agent returns a complex action for all its entities.
		/// </summary>
		void endTick(GameState& state) const;
		/// <summary>
		/// Default spawn entity method
		/// A list of On Spawn effects are executed just after spawning the entity
		/// </summary>
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
