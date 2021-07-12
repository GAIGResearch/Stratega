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

	/// <summary>
	/// Is the driving component of Stratega, with a provided gamestate it can generate a new set of available
	/// actions thanks to the <see cref="SGA::EntityActionSpace"/>.
	/// The returned actions store the action type and a list of sources and targets the action will be applied to.
	/// Each <see cref="SGA::RTSForwardModel"/> and <see cref="SGA::TBSForwardModel"/> by default contains the definition of how to advance the game
	/// and other utility methods used in each specific game type.
	/// When a player provides the forward model an action and a gamestate, it first checks the type of action
	/// and after it updates the action info before executing the action.
	/// The forward models can be extended by implementing forward model abstractions, can be useful for reducing the games
	/// complexity and ease the training and decision-making process of AI agents. 
	/// </summary>
	class ForwardModel
	{

	public:

		/// <summary>
		/// Effects applied on every tick of the game.
		/// </summary>
		std::vector<OnTickEffect> onTickEffects;

		/// <summary>
		/// Effects applied when a new entity is spawned in the game.
		/// </summary>
		std::vector<OnEntitySpawnEffect> onEntitySpawnEffects;

		/// <summary>
		/// Set of conditions required for a player to win the game.
		/// </summary>
		std::vector<std::vector<std::shared_ptr<Condition>>> winConditions;

		/// <summary>
		/// Set of conditions required for a player to lose the game.
		/// </summary>
		std::vector<std::vector<std::shared_ptr<Condition>>> loseConditions;
		
		//Constructor/destructor
		virtual ~ForwardModel() = default;
		ForwardModel();

		/// <summary>
		/// Advances the game state passed by parameter with a given action. 
		/// </summary>
		/// <param name="state">State to be advanced. The actual object is modified by this call.</param>
		/// <param name="action">Action to execute in the game state</param>
		virtual void advanceGameState(GameState& state, const Action& action) const = 0;

		/// <summary>
		/// Advances the game state passed by parameter with a given action
		/// </summary>
		/// <param name="state">Game state to advance. The actual object is modified by this call.</param>
		/// <param name="action">ActionAssignment to execute in the game state.</param>
		virtual void advanceGameState(GameState& state, const ActionAssignment& action) const = 0;
		
		/// <summary>
		/// Returns a list of available actions in the given gamestate by the received player
		/// </summary>
		/// <param name="state">Game state for which actions are generated.</param>
		/// <param name="playerID">ID of the players to generate available actions for.</param>
		/// <returns>The list of available actions for player with ID 'playerID' in game state 'state'.</returns>
		[[nodiscard]] std::vector<Action> generateActions(const GameState& state, int playerID) const;

		/// <summary>
		/// Generates actions in the given gamestate by the received player and fills the action vector passed by parameter.
		/// </summary>
		/// <param name="state">State of the game to generate actions for.</param>
		/// <param name="playerID">ID of the player for which actions are generated.</param>
		/// <param name="actionBucket">List of actions to populate by this method.</param>
		virtual void generateActions(const GameState& state, int playerID, std::vector<Action>& actionBucket) const;

		/// <summary>
		/// Clones the current forward model.
		/// </summary>
		/// <returns>A copy of this forward model.</returns>
		virtual std::unique_ptr<ForwardModel> clone() const = 0;


		/// <summary>
		/// Indicates the game type this forward model is for. GameType is an enum defined in GameState.h
		/// </summary>
		/// <returns>The game type of this forward model.</returns>
		virtual GameType getGameType() const = 0;

		/// <summary>
		/// Checks if a player has lost the game due to the game's lose conditions.
		/// </summary>
		/// <param name="state">State of the game to check whether the player has lost or not.</param>
		/// <param name="playerID">ID of the player to check their lose condition</param>
		/// <returns>True of the player lost.</returns>
		bool checkPlayerLost(const GameState& state, int playerID) const;

		/// <summary>
		/// Returns if a player won the game by attending to the winning conditions defined in the rules.
		/// </summary>
		/// <param name="state">State of the game to check whether the player won or not</param>
		/// <param name="playerID">ID of the player to be checked.</param>
		/// <returns>True if the player won</returns>
		bool checkPlayerWon(const GameState& state, int playerID) const;

		/// <summary>
		/// Spawns an entity in the game with the default spawn entity method. A list of On Spawn effects are 
		/// executed just after spawning the entity
		/// </summary>
		/// <param name="state">State to spawn the entity in.</param>
		/// <param name="entityType">Type of entity to spawn.</param>
		/// <param name="playerID">ID of the player that will own the entity.</param>
		/// <param name="position">Position where the entity will be spawned.</param>
		void spawnEntity(GameState& state, const EntityType& entityType, int playerID, const Vector2f& position) const;

		/// <summary>
		/// Returns the action space of this forward model
		/// </summary>
		/// <returns>Action space of this forward model.</returns>
		std::shared_ptr<EntityActionSpace> getActionSpace() const
		{
			return actionSpace;
		}

	protected:

		/// <summary>
		/// Action space (generator) for this forward model.
		/// </summary>
		std::shared_ptr<EntityActionSpace> actionSpace;


		// Not in use.
		///std::vector<std::pair<TargetType, std::vector<std::shared_ptr<Condition>>>> actionTargets;
		///virtual bool isValid(const GameState & state, const Action & action) const;

		/// <summary>
		/// Executes the action in the given game state. This is used by subclasses of this forward model to
		/// actually execute an action in the game.
		/// </summary>
		/// <param name="state">State where the action is executed. This game state will be altered.</param>
		/// <param name="action">Action to execute.</param>
		void executeAction(GameState& state, const Action& action) const;

		void executeAbortContinuousAction(GameState& state, const Action& action) const;
		void executeContinuousAction(GameState& state, const Action& action) const;
		void executeNormalAction(GameState& state, const Action& action) const;


		/// <summary>
		/// After each turn/ tick the triggers from the configuration are executed and
		/// the continuous action are executed after checking if they still are feasible.
		/// In terms of real-time strategy games, we consider a turn/tick to be the
		/// time-span until an agent returns a complex action for all its entities.
		/// </summary>
		/// <param name="state">State to finish its tick/turn.</param>
		void endTick(GameState& state) const;

		void executeOnTriggerEffects(GameState& state) const;
		void checkEntitiesContinuousActionIsComplete(GameState& state) const;
		void checkPlayerContinuousActionIsComplete(GameState& state) const;


		/// <summary>
		/// It generates a default action space unique pointer.
		/// </summary>
		/// <returns>Returns an empty action space.</returns>
		std::unique_ptr<EntityActionSpace> generateDefaultActionSpace() const
		{
			return std::make_unique<EntityActionSpace>();
		}

		/// <summary>
		/// Sets the action space of this objet to the oen passed by parameter.
		/// </summary>
		/// <param name="newActionSpace">Action space to set.</param>
		void setActionSpace(std::unique_ptr<EntityActionSpace> newActionSpace)
		{
			this->actionSpace = std::move(newActionSpace);
		}

	};

}
